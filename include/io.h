#ifndef IO_H
#define IO_H

#include <cmsis.h>

struct io_buffer {
    void *buffer;
    volatile uint32_t buffer_size;
    volatile int32_t buffer_index;
};

void console_write(void *, uint32_t);

#endif