#include <process.h>
#include <cmsis.h>
#include <list.h>
#include <syscalls.h>
#include <timer.h>
#include <mem.h>

struct proc_conf tsk_glob;
struct kernel_process kproc;
/* This varibale will be only read during context switch in assembly codes 
    and written during this file*/
uint32_t cur_proc_pointer __attribute__((used));

void user_end(void)
{
    ((struct process *)cur_proc_pointer)->state = 0;
    while (1);
}

void init_global_config(void)
{
    tsk_glob.uprocs = NULL;
    tsk_glob.nprocs = 0;
    tsk_glob.is_ready = 0;
    tsk_glob.available_pid = 0;
}

void init_kernel_proc(void (*func) (void))
{
    // There is no need to initialize the registers, because kproc always runs first
    kproc.spr = 0;
    kproc.ldr = 0;
    kproc.func = func;
    kproc.pid = -1;
    tsk_glob.nprocs++;
}

uint8_t add_proc_to_gloabl_list(struct process *proc)
{
    struct list *nn = list_node_create((void *)proc);
    tsk_glob.uprocs = list_add_node(tsk_glob.uprocs, nn);
    tsk_glob.nprocs++;
    return 1;
}

uint8_t get_pid(void)
{
    return tsk_glob.available_pid++;
}

uint32_t get_stack_address(uint32_t size)
{
    uint32_t sp;

    __ASM volatile (
        "mov %0, sp"
        : "=r"(sp)
    );
    return sp;
}

void run_procs(void)
{
    struct list *t;

    if (tsk_glob.nprocs == 1) {
        cur_proc_pointer = 0;
        return;
    }
    
    t = ((struct list *)(tsk_glob.uprocs));
    cur_proc_pointer = (uint32_t) &kproc;
    timer_start();
}

void select_next_proc(void)
{
    struct list *t;

    if (tsk_glob.nprocs == 2) {
        /* Should disable the timer and make the current_proc = kproc*/
        cur_proc_pointer = (uint32_t) &kproc;
        return;
    }
    else if (tsk_glob.nprocs == 1) {
        
    }

    t = (struct list *) tsk_glob.uprocs;
    /* for now i assume there are 2 procs*/
    if (t->data == (struct list *) cur_proc_pointer) {
        if (!(t->next))
            t = t->prev;
        else
            t = t->next;
    }
    cur_proc_pointer = (uint32_t) t->data;
}