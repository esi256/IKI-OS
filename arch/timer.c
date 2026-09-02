#include "cmsis.h"

uint8_t timer_initialize(void)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    return (SysTick->CALIB & SysTick_CALIB_NOREF_Msk) ? 0 : 1;
}

uint8_t timer_set(uint32_t ms_time)
{
    uint32_t count;

    if (ms_time < 10)
        return 0;
    if (!(SysTick->CALIB & SysTick_CALIB_TENMS_Msk))
        return 0;
    count = (SysTick->CALIB & SysTick_CALIB_TENMS_Msk) / 10 * ms_time;
    if (!count)
        return 0;
    SysTick->LOAD = count;
    return 1;
}

void timer_reset(void)
{
    uint32_t t;

    t = SysTick->CTRL;
    SysTick->VAL = 0;
}

void timer_enable(void)
{
    timer_reset();
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void timer_disable(void)
{
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}