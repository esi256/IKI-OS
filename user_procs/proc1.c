#include <syscalls.h>
#include <process.h>
#include <io.h>

void fun1(void)
{
    char m1[] = "hello world form fun1\n";
    char m2[] = "finished\n";
    char m3[] = "hello 2\n";
    char m4[] = "hello 3\n";
    write(m1, 23);
    write(m2, 10);
    write(m1, 23);
    // sleep(100);
    write(m3, 9);
    write(m4, 9);
    write(m4, 9);
}

void fun2(void)
{
    char m1[] = "hello world from fun2\n";
    write(m1, 23);
}

/* This function is part of the kernel */
void start(void)
{
    struct process *ts1 = init_user_proc(fun1);
    if (!ts1)
        console_write("did not work\n", 14);
    struct process *ts2 = init_user_proc(fun2);
    if (!ts2)
        console_write("did not work\n", 14);
    add_proc_to_gloabl_list(ts1);
    add_proc_to_gloabl_list(ts2);
}