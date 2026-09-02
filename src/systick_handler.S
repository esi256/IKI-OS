#include <cmsis.h>

  .extern cur_proc_pointer
  .extern system_ticks
  .extern earliest_deadline
  .extern wakeup_procs
  .extern 
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
  ldr r1, =earliest_deadline
  ldr r1, [r1]
  cmp r2, r1
  blo second_cond
  push {r2, lr}
  bl wakeup_procs
  pop {r2, lr}
  mov r0, #1
  b call_pendSV
second_cond:
  ldr r1, =timeslice_turn_tick
  ldr r1, [r1]
  cmp r2, r1
  blo exit_systick_handler
  mov r0, #0
call_pendSV:
  ldr r1, =SCB_BASE
  add r1, #4
  ldr r3, [r1]
  ldr r4, =SCB_ICSR_PENDSVSET_Msk
  orr r3, r4
  str r3, [r1]
exit_systick_handler:
  bx lr

  .global PendSV_Handler
  .type PendSV_Handler, %function
PendSV_Handler:
  CPSID I
  /* Save The Contents */
  /* if the procs can be run in priviliaged, this part should be written again*/
  mrs r0, psp
  stmdb r0!, {r4-r11}
  msr psp, r0
  ldr r1, =cur_proc_pointer
  ldr r1, [r1]
  str r0, [r1]
  str lr, [r1, #4]
  
/* Selcet the next process */
  mrs r0, msp
  str r0, [r0]
  bl select_next_proc
  ldr r0, =cur_proc_pointer
  ldr r0, [r0]
  ldr lr, [r0, #4]
  mov r1, lr
  /* Gettign the actual value of sp*/
  ldr r3, [r0]

  /* Load the Contents*/
  ldmia r3!, {r4-r11}
  cmp lr, #4
  ite ne
  msrne psp, r3
  msreq msp, r3
  bl timer_reset
  mov lr, r1
  CPSIE I
  bx lr