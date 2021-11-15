#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

void uart_init();
void uart_send_bit(u8 data);
void uart_send(const char* s);

#endif