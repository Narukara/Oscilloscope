#include "stm32f10x_usart.h"

void uart_send_bit(u8 data) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
    USART_SendData(USART1, data);
}

void uart_send(const char* s) {
    for (u8 i = 0; s[i] != '\0'; i++) {
        uart_send_bit(s[i]);
    }
}