#ifndef USART_H
#define USART_H

#include <io.h>

void usart1_transmit(struct io_buffer *usart1_buffer);
void usart1_transmit_init(void);

#endif