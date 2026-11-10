  .cpu cortex-m4
  .syntax unified
  .thumb

  .set SCB_BASE, 0xE000ED00
  .set SCB_ICSR_PENDSVSET_Msk, 0x10000000

  .section .text
  .extern cur_proc_pointer
  .extern system_ticks
  .extern earliest_wakeup_tick
  .extern wakeup_procs
  .extern timeslice_turn_tick
  .extern select_next_proc
  .extern timer_reset  
  .extern has_timeslice_turn_reached
  .extern is_proc_woken

  .global systick_handler
  .type systick_handler, %function
systick_handler:
  push {r2, lr}
  ldr r0, =system_ticks
  ldr r1, [r0]
  add r1, #1
  str r1, [r0]
  /* First Condition */
  bl is_proc_woken
  cmp r0, #1
  bne second_cond
  bl wakeup_procs
  b call_pendSV
second_cond:
  bl has_timeslice_turn_reached
  cmp r0, #1
  bne exit_systick_handler
call_pendSV:
  ldr r1, =SCB_BASE
  add r1, #4
  ldr r3, [r1]
  ldr r0, =SCB_ICSR_PENDSVSET_Msk
  orr r3, r0
  str r3, [r1]
exit_systick_handler:
  pop {r2,lr}
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
