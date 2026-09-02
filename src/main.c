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
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~(0xF << (2 * 6));
    GPIOB->MODER |= (0xA << (2 * 6));
    GPIOB->AFR[0] |= 0x77 << (4*6);

    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SVCall_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0x02);
    heap_region_allocate();
    init_global_config();
    init_kernel_proc(&kernel_main);
    timer_initialize();
    timer_set(10);
    syscalls_init();
    start();  
    run_procs();
    
    while (1);
}