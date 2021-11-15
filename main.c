#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "GUI.h"
#include "ILI9341.h"
#include "adc.h"
#include "spi.h"
#include "status.h"
#include "systick.h"
#include "tim.h"
#include "uart.h"

/**
 * ALL Resource
 *
 * GPIO
 * A0       ADC1    IN0
 * A1       ADC2    IN1
 * A2       ILI9341 RESET
 * A3       ILI9341 DC
 * A4       SPI     CS
 * A5       SPI     SCLK
 * A6       SPI     MISO
 * A7       SPI     MOSI
 * A9       UART1   TX
 * A10      UART1   RX
 * B1       TIM3    PWM
 *
 * USART1
 * ADC1
 * ADC2
 * SPI1
 * TIM2
 * TIM3
 */

void oscilloscope_init() {
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,
        ENABLE);
    tim_init();
    uart_init();
    adc_init();
    spi_init();
    ILI9341_init();
    GUI_init();
}

#define BUF_SIZE 1000
u8 buffer1[BUF_SIZE];
u8 buffer2[BUF_SIZE];

int main() {
    SCB->CCR |=
        SCB_CCR_STKALIGN_Msk;  // set STKALIGN, for Cortex-M3 r1p0 and r1p1
    oscilloscope_init();

    while (1) {
        __DSB();
        __WFI();
    }
}

void SysTick_Handler(void) {}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif