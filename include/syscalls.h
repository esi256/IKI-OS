#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <cmsis.h>

void syscalls_init(void);
extern void write(void *, uint32_t);
extern struct process *proc_init(void (*func)());
extern uint8_t proc_add(struct process *proc);
extern void timer_disable_call(void);
extern void timer_enable_call(void);
extern void sleep(uint32_t ticks);
extern void proc_remove(void);

#endif