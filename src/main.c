#include <syscalls.h>
#include <mem.h>
#include <mpu.h>
#include <process.h>
#include <uprocs.h>
#include <timer.h>
#include <io.h>
#include <kernel_error.h>
#include <usart.h>

void kernel_main(void)
{
    while (1);
}

int main(void)
{
    enum kernel_error err;
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SVCall_IRQn, 0xFF);
    NVIC_SetPriority(USART1_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0x01);
    usart1_transmit_init();
    if ((err = memory_map_init()) != ERR_OK)
        error_handler(err);
    init_global_config();
    init_idle_proc(&kernel_main);
    if ((err = timer_init()) != ERR_OK)
        error_handler(err);
    if ((err = timer_set(100)) != ERR_OK)
        error_handler(err);
    syscalls_init();
    start();  
    run_procs();
    
    while (1);
}