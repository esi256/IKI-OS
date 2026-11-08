#include <cmsis.h>
#include <kernel_error.h>

enum kernel_error timer_init(void)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    return (SysTick->CALIB & SysTick_CALIB_NOREF_Msk) ? ERR_TIMER_NO_REF : ERR_OK;
}

enum kernel_error timer_set(uint32_t ms_time)
{
    uint32_t count;

    if (ms_time < 10)
        return ERR_TIMER_BAD_PERIOD;
    if (!(SysTick->CALIB & SysTick_CALIB_TENMS_Msk))
        return ERR_TIMER_NO_CALIB;
    count = (SysTick->CALIB & SysTick_CALIB_TENMS_Msk) / 10 * ms_time;
    if (!count)
        return ERR_TIMER_BAD_COUNT;
    SysTick->LOAD = count;
    return ERR_OK;
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