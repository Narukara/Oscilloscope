#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

#include "systick.h"
#include "uart.h"

/**
 * PC13 is a LED
 */

int main() {
    SCB->CCR |=
        SCB_CCR_STKALIGN_Msk;  // set STKALIGN, for Cortex-M3 r1p0 and r1p1
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1,
        ENABLE);
    GPIO_Init(GPIOC, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_13,
                                         .GPIO_Speed = GPIO_Speed_2MHz,
                                         .GPIO_Mode = GPIO_Mode_Out_PP});
    // config USART1
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

    uart_send("Hi");

    while (1) {
        systick_delay_us(1000000);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
        systick_delay_us(1000000);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif