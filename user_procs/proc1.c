#include <syscalls.h>
#include <process.h>
#include <io.h>
#include <kernel_error.h>

void fun1(void)
{
    char m1[] = "hello world form fun1\n";
    while (1) {
        write(m1, 23);
        sleep(5);
    }
}

void fun2(void)
{
    char m1[] = "hello world from fun2\n";
    while (2) {
        write(m1, 23);
        sleep(5);
    }
}

/* 1. This function is part of the kernel 
    2. Check The return value of kernel functions */
void start(void)
{
    struct proc_setting setting;
    setting.func = fun1;
    setting.priority = 0;
    struct process *ts1 = init_user_proc(&setting);
    if (!ts1)
        error_handler(ERR_MEM_NO_SPACE_FOR_KERNEL);
    setting.func = fun2;
    setting.priority = 1;
    struct process *ts2 = init_user_proc(&setting);
    if (!ts2)
    error_handler(ERR_MEM_NO_SPACE_FOR_KERNEL);

    // error handling necessary
    add_proc_to_global_list(ts1);
    add_proc_to_global_list(ts2);
}