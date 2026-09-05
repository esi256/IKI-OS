#include <process.h>
#include <cmsis.h>
#include <list.h>
#include <syscalls.h>
#include <timer.h>
#include <mem.h>
#include <io.h>

extern uint32_t save_content(uint32_t spr, uint32_t psr, uint32_t pcr, uint32_t ldr);
// extern uint32_t get_control_reg(uint32_t)

struct proc_conf tsk_glob;
struct kernel_process kproc;
/* these varibales will be only 
    read during context switch 
    and written during this file */
uint32_t cur_proc_pointer;
uint32_t system_ticks;
uint32_t earliest_wakeup_tick;
uint32_t timeslice_turn_tick;
uint32_t schedule_state;

uint32_t compare_based_priority(struct list *p1, struct list *p2)
{
    struct process *t = (struct process *)p1->data;
    struct process *t1 = (struct process *)p2->data;

    return (t->priority >= t1->priority);
}

uint32_t compare_based_time(struct list *p1, struct list *p2)
{
    struct process *t = (struct process *)p1->data;
    struct process *t1 = (struct process *)p2->data;
    
    return (t->wakeup_tick <= t1->wakeup_tick);
}

/* This function address will be in lr register
    of users main function and is unprivilaged */
void user_end(void)
{
    ((struct process *)cur_proc_pointer)->state = 0;
    tsk_glob.nprocs--;
    if (tsk_glob.is_timer_pending) {
        timer_enable_call();
    }
    /* the space for process will not be delated */
    tsk_glob.ready_uprocs = list_rem_node(tsk_glob.ready_uprocs, tsk_glob.cur_proc);
    /* call the pendSV*/
    while (1);
}

void init_global_config(void)
{
    tsk_glob.ready_uprocs = NULL;
    tsk_glob.blocked_uprocs = NULL;
    tsk_glob.cur_proc = NULL;
    tsk_glob.nprocs = 0;
    tsk_glob.is_timer_pending = 1;
    tsk_glob.available_pid = 0;

    system_ticks = 0;
    earliest_wakeup_tick = NO_PROCESS_TO_WAKUP;
    timeslice_turn_tick = 0;
}

void init_kernel_proc(void (*func) (void))
{
    /* There is no need to initialize the registers, 
        because kproc always runs first */
    kproc.spr = 0;
    kproc.ldr = 0;
    kproc.ctrlr = 0;
    kproc.func = func;
    kproc.pid = -1;
    tsk_glob.nprocs++;
}

struct process *init_user_proc(void (*func)(void))
{
    /* add the error handling */
    struct process *tsk;
    uint32_t pcr = 0, psr = 0, tsz = sizeof(struct process);

    if (!(tsk = kalloc(&tsz)))
        return NULL;
    tsk->func = func;
    /* change this shit to be compatible with qemu */
    tsk->pid = get_pid() + 1;
    tsk->priority = 0;
    tsk->state = 1;
    tsk->stack_size = 2048;
    tsk->stack_base_addr = ((uint32_t) uproc_stack_allocate(tsk->pid ,&tsk->stack_size));
    if (!tsk->stack_base_addr)
        return NULL;
    tsk->spr =  tsk->stack_base_addr + tsk->stack_size - sizeof(uint32_t);
    tsk->ldr = 0xFFFFFFFD;
    tsk->ctrlr = 0x03;
    pcr = (uint32_t) func;
    psr = (1 << 24);
    tsk->spr = save_content(tsk->spr, psr, pcr, (uint32_t) user_end);
    return tsk;
}

uint8_t add_proc_to_gloabl_list(struct process *proc)
{
    struct list *nn = list_node_create((void *)proc);
    tsk_glob.ready_uprocs = list_add_node_func_based(tsk_glob.ready_uprocs, nn, compare_based_priority);
    tsk_glob.nprocs++;
    return 1;
}

uint8_t get_pid(void)
{
    return tsk_glob.available_pid++;
}

uint32_t get_stack_address(void)
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
        tsk_glob.cur_proc = NULL;
        return;
    }
    cur_proc_pointer = (uint32_t) &kproc;
    tsk_glob.is_timer_pending = 0;
    /* should make this as a config and make structures to turn tick to s */
    timeslice_turn_tick = 1;
    schedule_state = NO_CURRENT_PROC_STATE;
    timer_enable();
}

void suspend_cur_proc(uint32_t ticks)
{
    struct list *t;

    ((struct process *)cur_proc_pointer)->state = proc_blocked;
    ((struct process *)cur_proc_pointer)->wakeup_tick = system_ticks + ticks;
    t = tsk_glob.cur_proc;
    tsk_glob.ready_uprocs = list_rem_node(tsk_glob.ready_uprocs ,tsk_glob.cur_proc);
    tsk_glob.blocked_uprocs = list_add_node_func_based(tsk_glob.blocked_uprocs, t, compare_based_time);
    if (earliest_wakeup_tick < ((struct process *)cur_proc_pointer)->wakeup_tick || earliest_wakeup_tick == NO_PROCESS_TO_WAKUP)
        earliest_wakeup_tick = ((struct process *)cur_proc_pointer)->wakeup_tick;

    tsk_glob.cur_proc = NULL;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/* This function will be only called when a task is going to be waken up 
    so there is no need for checking because it is already done */
void wakeup_procs(void)
{
    struct list *t;
    
    ((struct process *)tsk_glob.blocked_uprocs->data)->wakeup_tick = 0;
    ((struct process *)tsk_glob.blocked_uprocs->data)->state = proc_ready;
    t = tsk_glob.blocked_uprocs;
    tsk_glob.blocked_uprocs = list_rem_head(tsk_glob.blocked_uprocs);
    tsk_glob.ready_uprocs = list_add_node_func_based(tsk_glob.ready_uprocs , t, compare_based_priority);
    if (tsk_glob.blocked_uprocs)
        earliest_wakeup_tick = ((struct process *)tsk_glob.blocked_uprocs->data)->wakeup_tick;
    else
        earliest_wakeup_tick = NO_PROCESS_TO_WAKUP;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void select_next_proc(uint32_t is_proc_waken_up)
{
    struct process *pt, *pt1;

    if (is_proc_waken_up) {
        pt = ((struct process *)tsk_glob.cur_proc);
        pt1 = ((struct process *) tsk_glob.ready_uprocs->data);
        if (pt1->priority > pt->priority) {
            cur_proc_pointer = (uint32_t)tsk_glob.ready_uprocs->data;
            tsk_glob.cur_proc = tsk_glob.ready_uprocs;
            timeslice_turn_tick = system_ticks + 10;
            return;
        }
        /* if their priority were equal don't change anything */
        return;
    }
    /* todo: change this part to cope with the blocked procs */
    if (tsk_glob.nprocs == 1) {
        cur_proc_pointer = (uint32_t) &kproc;
        tsk_glob.cur_proc = NULL;
        return;
    }
    /* change the blow code to deal with nreadyprocs*/
    if (!tsk_glob.cur_proc || !tsk_glob.cur_proc->next) {
        if (tsk_glob.cur_proc == tsk_glob.ready_uprocs)
            return;
        cur_proc_pointer = (uint32_t) tsk_glob.ready_uprocs->data;
        tsk_glob.cur_proc = tsk_glob.ready_uprocs;
        timeslice_turn_tick = system_ticks + 10;
        return;
    }
    /* not the last proc in the ready list */
    pt = ((struct process *)tsk_glob.cur_proc->data);
    pt1 = ((struct process *)tsk_glob.cur_proc->next->data);
    if (pt->priority <= pt1->priority) {
        cur_proc_pointer = (uint32_t) tsk_glob.cur_proc->next->data;
        tsk_glob.cur_proc = tsk_glob.cur_proc->next;
        return;
    }
    if (tsk_glob.cur_proc != tsk_glob.ready_uprocs) {
        cur_proc_pointer = (uint32_t) tsk_glob.ready_uprocs->data;
        tsk_glob.cur_proc = tsk_glob.ready_uprocs;
    }
}