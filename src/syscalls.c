#include <cmsis.h>
#include <buffer.h>
#include <process.h>
#include <mem.h>
#include <io.h>
#include <timer.h>

uint32_t systemcall_addrs[6];

void syscalls_init(void)
{
    systemcall_addrs[0] = (uint32_t) &console_write;
    systemcall_addrs[1] = (uint32_t) &init_user_proc;
    systemcall_addrs[2] = (uint32_t) &add_proc_to_gloabl_list;
    systemcall_addrs[3] = (uint32_t) &timer_enable;
    systemcall_addrs[4] = (uint32_t) &timer_disable;
    systemcall_addrs[5] = (uint32_t) &suspend_cur_proc;
    /* to be continued */
}