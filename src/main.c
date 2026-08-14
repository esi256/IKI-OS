#include <syscalls.h>
#include <mem.h>
#include <mpu.h>
#include <process.h>
#include <task1.h>
#include <timer.h>

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
    char buf[] = "timer error\n";

    heap_region_allocate();
    init_global_config();
    init_kernel_proc(&kernel_main);
    if (!timer_initialize()) {
        kwrite(buf, 13);
    }
    timer_set(80);

    start(); 
    
    if (tsk_glob.is_ready)
        run_procs();
    
    while (1);
}