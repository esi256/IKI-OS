    .cpu cortex-m4
    .syntax unified
    .thumb

    .global floor_pow2
    .type floor_pow2, %function
floor_pow2:
    clz r0, r0
    bx lr

    .global round_up_pow2
    .type round_up_pow2, %function
round_up_pow2:
    cmp r0, #0
    beq exit_ceil_pow2
    sub r0, #1
    clz r1, r0
    mov r2, #32
    sub r1, r2, r1
    mov r0, #1
    lsl r0, r1
exit_ceil_pow2:
    bx lr
