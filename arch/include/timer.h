#include "cmsis.h"

enum kernel_error timer_init(void);
enum kernel_error timer_set(uint32_t ms_time);
void timer_enable(void);
void timer_disable(void);