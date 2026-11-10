#include <process.h>
#include <cmsis.h>
#include <list.h>
#include <syscalls.h>
#include <timer.h>
#include <mem.h>
#include <io.h>

extern uint32_t save_content(uint32_t spr, uint32_t psr, uint32_t pcr, uint32_t ldr);
// extern uint32_t get_control_reg(uint32_t)

struct proc_global_config tsk_glob;
struct idle_proc kproc;
/* these varibales will be only 
    read during context switch 
    and written during this file */
uint32_t cur_proc_pointer;
uint32_t system_ticks;
uint32_t earliest_wakeup_tick;
uint32_t timeslice_turn_tick;
uint32_t context_switch_status;

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
    proc_remove();
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
    timeslice_turn_tick = NO_PROCESS_TO_TIMESLICE;
}

void init_idle_proc(void (*func) (void))
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

struct process *init_user_proc(struct proc_setting *st)
{
    /* add the error handling */
    struct process *tsk;
    uint32_t pcr = 0, psr = 0, tsz = sizeof(struct process);

    if (!(tsk = kalloc(&tsz)))
        return NULL;
    tsk->func = st->func;
    /* change this shit to be compatible with MPU and memory managment*/
    tsk->pid = get_pid() + 3;
    tsk->priority = st->priority;
    tsk->state = 1;
    tsz = sizeof(struct io_buffer);
    tsk->buffer = (struct io_buffer *) kalloc(&tsz);
    tsk->stack_size = 2048;
    tsk->stack_base_addr = ((uint32_t) uproc_stack_allocate(tsk->pid ,&tsk->stack_size));
    if (!tsk->stack_base_addr)
        return NULL;
    tsk->spr =  tsk->stack_base_addr + tsk->stack_size - sizeof(uint32_t);
    tsk->ldr = 0xFFFFFFFD;
    tsk->ctrlr = 0x03;
    pcr = (uint32_t) st->func;
    psr = (1 << 24);
    tsk->spr = save_content(tsk->spr, psr, pcr, (uint32_t) user_end);
    return tsk;
}

uint8_t add_proc_to_global_list(struct process *proc)
{
    __disable_irq();
    struct list *nn = list_node_create((void *)proc);
    tsk_glob.ready_uprocs = list_add_node_func_based(tsk_glob.ready_uprocs, nn, compare_based_priority);
    tsk_glob.nprocs++;
    __enable_irq();
    return 1;
}

uint8_t get_pid(void)
{
    return tsk_glob.available_pid++;
}

void call_imm_pendsv()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void run_procs(void)
{
    struct list *t;

    if (tsk_glob.nprocs == 1) {
        cur_proc_pointer = 0;
        tsk_glob.cur_proc = NULL;
        context_switch_status = cs_st_no_proc_exist;
        return;
    }
    /* should make this as a config and make structures to turn tick to s */
    timeslice_turn_tick = 1;
    earliest_wakeup_tick = NO_PROCESS_TO_WAKUP;
    context_switch_status = cs_st_initial_state;
    cur_proc_pointer = (uint32_t) &kproc;
    tsk_glob.cur_proc = NULL;
    tsk_glob.is_timer_pending = 0;
    timer_enable();
    call_imm_pendsv();
}

void proc_detach(void)
{
    __disable_irq();
    ((struct process *)cur_proc_pointer)->state = 0;
    tsk_glob.nprocs--;
    /* the space for process will not be freed */
    tsk_glob.ready_uprocs = list_rem_node(tsk_glob.ready_uprocs, tsk_glob.cur_proc);
    /* make a change for state that current proc should change */
    tsk_glob.cur_proc = NULL;
    if (!tsk_glob.ready_uprocs)
        context_switch_status = cs_st_no_ready_proc_exist;
    else
        context_switch_status = cs_st_cur_process_is_removed;
    call_imm_pendsv();
    __enable_irq();
}

void suspend_cur_proc(uint32_t ticks)
{
    struct list *t;

    __disable_irq();
    ((struct process *)cur_proc_pointer)->state = proc_blocked;
    ((struct process *)cur_proc_pointer)->wakeup_tick = system_ticks + ticks;
    t = tsk_glob.cur_proc;
    tsk_glob.ready_uprocs = list_rem_node(tsk_glob.ready_uprocs ,tsk_glob.cur_proc);
    tsk_glob.blocked_uprocs = list_add_node_func_based(tsk_glob.blocked_uprocs, t, compare_based_time);
    if (earliest_wakeup_tick > ((struct process *)cur_proc_pointer)->wakeup_tick || earliest_wakeup_tick == NO_PROCESS_TO_WAKUP)
        earliest_wakeup_tick = ((struct process *)cur_proc_pointer)->wakeup_tick;
    tsk_glob.cur_proc = NULL;
    if (!tsk_glob.ready_uprocs)
        context_switch_status = cs_st_no_ready_proc_exist;
    else
        context_switch_status = cs_st_cur_process_is_suspended;
    call_imm_pendsv();
    __enable_irq();
}

/* This function will be only called when a task is going to be waken up 
    so there is no need for checking because it is already done */
void wakeup_procs(void)
{
    struct list *t;
    struct process *tp;

    t = tsk_glob.blocked_uprocs;
    while (t) {
        tp = t->data;
        if (tp->wakeup_tick > system_ticks)
            break;
        tp->wakeup_tick = 0;
        tp->state = proc_ready;
        tsk_glob.blocked_uprocs = list_rem_head(tsk_glob.blocked_uprocs);
        tsk_glob.ready_uprocs = list_add_node_func_based(tsk_glob.ready_uprocs , t, compare_based_priority);
        t = tsk_glob.blocked_uprocs;
    }
    if (tsk_glob.blocked_uprocs)
        earliest_wakeup_tick = ((struct process *)tsk_glob.blocked_uprocs->data)->wakeup_tick;
    else
        earliest_wakeup_tick = NO_PROCESS_TO_WAKUP;
}

uint8_t has_timeslice_turn_reached(void)
{
    if (system_ticks >= timeslice_turn_tick) {
            context_switch_status = cs_st_time_limit_has_reached;
        return 1;
    }
    return 0;
}

uint8_t is_proc_woken(void)
{
    if (earliest_wakeup_tick != NO_PROCESS_TO_WAKUP && system_ticks >= earliest_wakeup_tick) {
        context_switch_status = cs_st_a_process_is_woken;
        return 1;
    }
    return 0;
}

void switch_to(struct list *node)
{
    cur_proc_pointer = (uint32_t) node->data;
    tsk_glob.cur_proc = node;
}

// fix the 10 hardcode
void select_next_proc()
{
    struct process *pt, *pt1;
    struct list *l, *l1;
    uint8_t time_tick_flag = 1;

    switch (context_switch_status) {
        case cs_st_initial_state:
        case cs_st_cur_process_is_suspended:
        case cs_st_cur_process_is_removed:
            switch_to(tsk_glob.ready_uprocs);
            break;
        case cs_st_no_ready_proc_exist:
        case cs_st_no_proc_exist:
            cur_proc_pointer = (uint32_t) &kproc;
            tsk_glob.cur_proc = NULL;
            timeslice_turn_tick = NO_PROCESS_TO_TIMESLICE;   /* no slice while idle */
            time_tick_flag = 0;           
            break;
        case cs_st_a_process_is_woken:
            pt1 = tsk_glob.ready_uprocs->data;
            // cur_proc == NULL means idle: always take the woken process
            if (!tsk_glob.cur_proc ||
                pt1->priority > ((struct process *) tsk_glob.cur_proc->data)->priority) {
                switch_to(tsk_glob.ready_uprocs);
            } 
            else {
                time_tick_flag = 0;
            }
            break;
        case cs_st_time_limit_has_reached:
            if (!tsk_glob.cur_proc) {    /* idle: nothing to rotate */
                time_tick_flag = 0;
                break;
            }
            l1  = tsk_glob.cur_proc->next;
            pt  = tsk_glob.ready_uprocs->data;     /* head = top priority group */
            if (l1 && ((struct process *) l1->data)->priority == pt->priority)
                tsk_glob.cur_proc = l1;                        /* next in group */
            else
                tsk_glob.cur_proc = tsk_glob.ready_uprocs;     /* wrap to head */
            cur_proc_pointer = (uint32_t) tsk_glob.cur_proc->data;
            break;
    }
    if (time_tick_flag)
        timeslice_turn_tick = system_ticks + 1;
}