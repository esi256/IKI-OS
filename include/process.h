#include <cmsis.h>

/* The order of struct should not be changed, 
    It's importent in context switch in assembly files*/
struct process {
    uint32_t spr;
    uint32_t ldr;
    // uint32_t pcr;
    // uint32_t psr;
    void (*func) (void);
    int8_t pid;
    int8_t priority;
    int8_t state;
    int8_t nop2;
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
    struct list *uprocs;
    uint8_t nprocs;
    uint8_t is_ready;
    uint8_t available_pid;
};

extern struct proc_conf tsk_glob;

uint8_t get_pid(void);
void init_global_config(void);
void init_kernel_proc(void (*) (void));
uint32_t get_stack_address(uint32_t size);
uint8_t add_proc_to_gloabl_list(struct process *proc);
void run_procs(void);
