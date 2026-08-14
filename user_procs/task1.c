#include <syscalls.h>
#include <process.h>

void fun1(void)
{
    char m1[] = "hello world form fun1\n";
    char m2[] = "finished\n";
    char m3[] = "hello 2\n";
    char m4[] = "hello 3\n";
    kwrite(m1, 23);
    kwrite(m2, 10);
    kwrite(m1, 23);
    kwrite(m3, 9);
    kwrite(m4, 9);
    kwrite(m4, 9);
}

void fun2(void)
{
    char m1[] = "hello world from fun2\n";
    kwrite(m1, 23);
}

void start(void)
{
    struct process *ts1 = init_task(fun1);
    struct process *ts2 = init_task(fun2);
    add_task(ts1);
    add_task(ts2);
    fin_task();
}