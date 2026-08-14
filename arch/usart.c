#include "usart.h"
#include "buffer.h"

void usart_configure(USART_TypeDef *USARTx)
{
    USARTx->CR1 &= ~USART_CR1_UE;
    USARTx->CR2 &= ~USART_CR2_STOP;
    USARTx->CR1 &= ~USART_CR1_PCE;
    USARTx->CR1 &= ~USART_CR1_OVER8;
    USARTx->BRR = 0x208d;
    USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USARTx->CR1 &= ~USART_CR1_TXEIE;
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
    USARTx->CR1 |= USART_CR1_UE;
}

void usart_write(USART_TypeDef *USARTx, struct usart_buffer *usartx_buffer)
{
    if (!(USARTx->SR & USART_SR_TXE))
        return;

    void *data = usartx_buffer->buffer;

    if (usartx_buffer->buffer_index <= usartx_buffer->buffer_size - 1) {
        USARTx->DR = ((char *)data)[usartx_buffer->buffer_index];
        usartx_buffer->buffer_index++;
    }
    else {
        usartx_buffer->buffer_index = 0;
        USARTx->CR1 &= ~USART_CR1_TXEIE;
    }
}

void usart1_transmit_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    usart_configure(USART1);
}

void usart1_transmit(void)
{
    USART1->CR1 |= USART_CR1_TXEIE;
}

void USART1_IRQHandler()
{
    usart_write(USART1, &usart1_buffer);
}