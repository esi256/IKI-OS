#include <cmsis.h>
#include <usart.h>
#include <buffer.h>
#include <process.h>
#include <mem.h>

extern uint32_t save_content(uint32_t spr, uint32_t psr, uint32_t pcr, uint32_t ldr);
extern void user_end(void);

void kwrite(void *buff, uint32_t len)
{
    usart1_buffer.buffer = buff;
    usart1_buffer.buffer_index = 0;
    usart1_buffer.buffer_size = len;

    usart1_transmit();
}

struct process *init_task(void (*func)(void))
{
    struct process *tsk = (struct process *) ualloc(sizeof(struct process));
    uint32_t pcr = 0, psr = 0;

    if (!tsk)
        return NULL;
    tsk->func = func;
    tsk->pid = get_pid();
    tsk->priority = 0;
    tsk->state = 1;
    tsk->stack_size = 2048;
    tsk->stack_base_addr = ((uint32_t) ualloc(tsk->stack_size) & 0xffffff00);
    tsk->spr =  tsk->stack_base_addr;
    tsk->ldr = 0xFFFFFFFD;
    pcr = (uint32_t) func;
    psr = (1 << 24);
    tsk->spr = save_content(tsk->spr, psr, pcr, (uint32_t) user_end);
    return tsk;
}

uint8_t add_task(struct process *proc)
{
    add_proc_to_gloabl_list(proc);
}

uint8_t rem_task(uint8_t pid)
{
    return 0;
}

uint8_t fin_task()
{
    tsk_glob.is_ready = 1;
}