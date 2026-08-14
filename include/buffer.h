#include <cmsis.h>

struct usart_buffer {
    void *buffer;
    volatile uint32_t buffer_size;
    volatile uint32_t buffer_index;
};

extern struct usart_buffer usart1_buffer;