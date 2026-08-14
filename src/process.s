    .cpu cortex-m4
    .syntax unified
    .thumb

    .global save_content
    .type save_content, %function
save_content:
    str r1, [r0], #-4
    str r2, [r0], #-4
    str r3, [r0], #-4
    mov r3, #0
    mov r2, #0
save_zeros:
    cmp r3, #13
    beq exit
    str r2, [r0], #-4
    add r3, #1
    b   save_zeros
exit:
    add r0, r0, #4
    /* Should change this line to support privilaged and unprivilaged tasks */
    msr psp, r0
    bx lr 
