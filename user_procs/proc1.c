#include <syscalls.h>
#include <process.h>
#include <io.h>

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

/* This function is part of the kernel */
void start(void)
{
    struct proc_setting setting;
    setting.func = fun1;
    setting.priority = 0;
    struct process *ts1 = init_user_proc(&setting);
    setting.func = fun2;
    setting.priority = 1;
    struct process *ts2 = init_user_proc(&setting);
    add_proc_to_gloabl_list(ts1);
    add_proc_to_gloabl_list(ts2);
}