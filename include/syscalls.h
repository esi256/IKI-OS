#include <cmsis.h>

extern void kwrite(void *buff, uint32_t len);
struct process *init_task(void (*func)(void));
uint8_t add_task(struct process *proc);
uint8_t fin_task();
