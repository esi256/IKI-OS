#include <io.h>
#include <buffer.h>
#include <usart.h>

void console_write(void *buff, uint32_t len)
{
    usart1_buffer.buffer = buff;
    usart1_buffer.buffer_index = 0;
    usart1_buffer.buffer_size = len;

    usart1_transmit();
}