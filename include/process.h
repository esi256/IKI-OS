#include <cmsis.h>

enum proc_states {
    proc_blocked,
    proc_ready
};

/* The order of struct should not be changed, 
    It's importent in context switch in assembly files*/
struct process {
    uint32_t spr;
    uint32_t ldr;
    void (*func) (void);
    int8_t pid;
    int8_t priority;
    int8_t state;
    int8_t nop2;
    uint32_t wakeup_tick;
    uint32_t stack_base_addr;
    uint32_t stack_size;
};

struct kernel_process {
    uint32_t spr;
    uint32_t ldr;
    void (*func) (void);
    int8_t pid;
};

struct proc_conf {
    struct list *ready_uprocs;
    struct list *blocked_uprocs;
    struct list *cur_proc;
    uint8_t nprocs;
    uint8_t is_timer_pending;
    uint8_t available_pid;
};

extern struct proc_conf tsk_glob;

uint8_t get_pid(void);
void init_global_config(void);
void init_kernel_proc(void (*) (void));
struct process *init_user_proc(void (*func)(void));
uint32_t get_stack_address(void);
uint8_t add_proc_to_gloabl_list(struct process *proc);
void run_procs(void);
