    .cpu cortex-m4
    .syntax unified
    .thumb

    .extern systemcall_addrs

    .global svc_handler
    .type svc_handler, %function
    /* if i had to add periodic processes, i should change this, like i should not use cpside*/
svc_handler:
    cpsid I
    mrs r0, control
    eor r0, #0x01
    msr control, r0
    cpsie I
    bx lr

    .global write
    .type write, %function
write:
    svc 0x00
    push {lr}
    blx console_write
    pop {lr}
    svc 0x00
    bx lr

    .global proc_init
    .type proc_init, %function
proc_init:
    svc 0x00
    push {lr}
    blx init_user_proc
    pop {lr}
    svc 0x00
    bx lr

    .global proc_add
    .type proc_add, %function
proc_add:
    svc 0x00
    push {lr}
    blx add_proc_to_gloabl_list
    svc 0x00
    pop {lr}
    bx lr
    
    .global proc_remove
    .type proc_remove, %function
proc_remove:
    svc 0x00
    push {lr}
    blx proc_detach
    svc 0x00
    pop {lr}
    bx lr

    .global timer_enable_call
    .type timer_enable_call, %function
timer_enable_call:
    svc 0x00
    push {lr}
    blx timer_enable
    svc 0x00
    pop {lr}
    bx lr

    .global timer_disable_call
    .type timer_disable_call, %function
timer_disable_call:
    svc 0x00
    push {lr}
    blx timer_disable
    pop {lr}
    svc 0x00
    bx lr;

    .global sleep
    .type sleep, %function
sleep:
    svc 0x00
    push {lr}
    blx suspend_cur_proc
    svc 0x00
    pop {lr}
    bx lr
    