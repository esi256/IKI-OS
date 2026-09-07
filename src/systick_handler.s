  .cpu cortex-m4
  .syntax unified
  .thumb

  .set SCB_BASE, 0xE000ED00
  .set SCB_ICSR_PENDSVSET_Msk, 0x10000000

  .section .bss
  .align 4
status:
  .word 0

  .section .text
  .extern cur_proc_pointer
  .extern system_ticks
  .extern earliest_wakeup_tick
  .extern wakeup_procs
  .extern timeslice_turn_tick
  .extern select_next_proc
  .extern timer_reset  

  .global systick_handler
  .type systick_handler, %function
systick_handler:
  ldr r1, =system_ticks
  ldr r2, [r1]
  add r2, #1
  str r2, [r1]
  ldr r1, =earliest_wakeup_tick
  ldr r1, [r1]
  cmp r2, r1
  blo second_cond
  push {r2, lr}
  bl wakeup_procs
  pop {r2, lr}
  mov r0, #1
  ldr r3, =status
  str r0, [r3]
  b call_pendSV
second_cond:
  ldr r1, =timeslice_turn_tick
  ldr r1, [r1]
  cmp r2, r1
  blo exit_systick_handler
  mov r0, #0
  ldr r3, =status
  str r0, [r3]  
call_pendSV:
  ldr r1, =SCB_BASE
  add r1, #4
  ldr r3, [r1]
  ldr r0, =SCB_ICSR_PENDSVSET_Msk
  orr r3, r0
  str r3, [r1]

exit_systick_handler:
  bx lr

  .global PendSV_Handler
  .type PendSV_Handler, %function
PendSV_Handler:
  CPSID I
  /* Save The Contents */
  tst lr, #4
  ite ne
  mrsne r0, psp
  mrseq r0, msp
  stmdb r0!, {r4-r11}
  ite ne
  msrne psp, r0
  msreq msp, r0
  ldr r1, =cur_proc_pointer
  ldr r1, [r1]
  str r0, [r1]
  str lr, [r1, #4]
  mrs r0, control
  str r0, [r1, #8]
  
/* Selcet the next process */
  ldr r0, =status
  ldr r0, [r0]
  bl select_next_proc
  ldr r0, =cur_proc_pointer
  ldr r0, [r0]

  /* Load the Contents*/
  ldr lr, [r0, #4]
  ldr r2, [r0, #8]
  /* Gettign the actual value of sp*/
  ldr r3, [r0]
  ldmia r3!, {r4-r11}
  tst lr, #4
  ite ne
  msrne psp, r3
  msreq msp, r3
  push {r2, lr}
  bl timer_reset
  pop {r2, lr}
  CPSIE I
  msr control, r2
  isb
  bx lr  
  