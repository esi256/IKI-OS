#include <cmsis.h>

extern uint32_t systemcall_addrs[5];

void syscalls_init(void);
extern void write(void *, uint32_t);
extern struct process *proc_init(void (*func)());
extern uint8_t proc_add(struct process *proc);
extern void timer_disable_call(void);
extern void timer_enable_call(void);
extern void sleep(uint32_t ticks);
