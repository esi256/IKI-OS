    .cpu cortex-m4
    .syntax unified
    .thumb

    .extern systemcall_addrs

    .global svc_handler
    .type svc_handler, %function
/* if i had to add periodic processes, i should change this, like i should not use cpside*/
svc_handler:
    cpsid I
    push {r4-r8, lr}
    /* here I assumed that process which asked for systemcall were using psp */
    mrs r4, psp
    /* read the pc register */
    ldr r5, [r4, #24]
    sub r5, r5, #2
    /* calculate the offset of array of system calls */
    ldrb r5, [r5]
    ldr r6, =systemcall_addrs
    add r6, r6, r5
    ldr r6, [r6]
    /* puts the arguments */
    ldr r0, [r4]
    ldr r1, [r4, #4]
    ldr r2, [r4, #8]
    blx r6
    pop  {r4-r8, lr}
    /* should handle return value */
    cpsie I
    bx lr

    .global write
    .type write, %function
write:
    svc 0x00
    bx lr
    
    .global proc_init
    .type proc_init, %function
proc_init:
    svc 0x04
    bx lr

    .global proc_add
    .type proc_add, %function
proc_add:
    svc 0x08
    bx lr

    .global timer_enable_call
    .type timer_enable_call, %function
timer_enable_call:
    svc 0x0c
    bx lr

    .global timer_disable_call
    .type timer_disable_call, %function
timer_disable_call:
    svc 0x10;
    bx lr;
