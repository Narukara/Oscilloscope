#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

void uart_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_9,  // TX
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_AF_PP});
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_10,  // RX
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_IN_FLOATING});
    USART_Init(USART1, &(USART_InitTypeDef){
                           .USART_BaudRate = 115200,
                           .USART_WordLength = USART_WordLength_9b,
                           .USART_StopBits = USART_StopBits_1,
                           .USART_Parity = USART_Parity_Odd,
                           .USART_Mode = USART_Mode_Rx | USART_Mode_Tx});
    USART_Cmd(USART1, ENABLE);
}

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