#include <io.h>
#include <usart.h>
#include <process.h>

void console_write(void *buff, uint32_t len)
{
    struct process *cur_tsk = tsk_glob.cur_proc->data;

    if (!cur_tsk) {
        struct io_buffer t = {.buffer = buff, .buffer_index = 0, .buffer_size = len};
        usart1_transmit(&t);
        return;
    }
    cur_tsk->buffer->buffer = buff;
    cur_tsk->buffer->buffer_index = 0;
    cur_tsk->buffer->buffer_size = len;

    usart1_transmit(cur_tsk->buffer);
}