    .cpu cortex-m4
    .syntax unified
    .thumb

    .extern systemcall_addrs
    .extern cur_proc_pointer

    .global svc_handler
    .type svc_handler, %function
    /* if i had to add periodic processes, i should change this, like i should not use cpside*/
svc_handler:
    cpsid I
    push {r4, r5, lr}
    tst lr, #4
    ite eq
    mrseq r4, msp
    mrsne r4, psp
    ldr r1, [r4, #24]
    ldrb r1, [r1, #-2]
    /* check if index is correct */
    cmp r1, #7
    bhs exit_svc_handler
    /* Calculating the address of system call*/
    mov r0, #4
    mul r1, r0
    ldr r2, =systemcall_addrs
    add r2, r1
    ldr r5, [r2]
    /* restore the possible arguments */
    ldmia r4, {r0-r3}
    /* call the system call and save the return value*/
    blx r5
    str r4, [r0]
exit_svc_handler:
    pop {r4, r5, lr}
    cpsie I
    bx lr

    .global write
    .type write, %function
write:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x00
    blxne console_write
    pop {r4, lr}   
    bx lr

    .global proc_init
    .type proc_init, %function
proc_init:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x04
    blxne init_user_proc
    pop {r4, lr}   
    bx lr

    .global proc_add
    .type proc_add, %function
proc_add:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x08
    blxne add_proc_to_global_list
    pop {r4, lr}   
    bx lr
    
    .global proc_remove
    .type proc_remove, %function
proc_remove:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x0c
    blxne proc_detach
    pop {r4, lr}   
    bx lr

    .global timer_enable_call
    .type timer_enable_call, %function
timer_enable_call:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x10
    blxne timer_enable
    pop {r4, lr}   
    bx lr

    .global timer_disable_call
    .type timer_disable_call, %function
timer_disable_call:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x14
    blxne timer_disable
    pop {r4, lr}   
    bx lr

    .global sleep
    .type sleep, %function
sleep:
    push {r4, lr}
    mrs r4, control
    and r4, #1
    cmp r4, #1
    ite eq
    svceq 0x18
    blxne suspend_cur_proc
    pop {r4, lr}   
    bx lr
