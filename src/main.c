#include <syscalls.h>
#include <mem.h>
#include <mpu.h>
#include <process.h>
#include <uprocs.h>
#include <timer.h>
#include <io.h>

void kernel_main(void)
{
    while (1);
}

int main(void)
{
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SVCall_IRQn, 0xFF);
    NVIC_SetPriority(USART1_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0x01);
    
    if (!memory_map_init()) {
        console_write("not\n", 5);
    }
    init_global_config();
    init_kernel_proc(&kernel_main);
    timer_init();
    timer_set(100);
    syscalls_init();
    start();  
    run_procs();
    
    while (1);
}