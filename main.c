#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_usart.h"

#include "GUI.h"
#include "ILI9341.h"
#include "adc.h"
#include "spi.h"
#include "systick.h"
#include "uart.h"

int main() {
    SCB->CCR |=
        SCB_CCR_STKALIGN_Msk;  // set STKALIGN, for Cortex-M3 r1p0 and r1p1

    uart_init();
    uart_send("Hi");
    adc_init();
    spi_init();
    ILI9341_init();
    GUI_init();

    while (1) {
        __DSB();
        __WFI();
    }

    SysTick->LOAD = 7 * 10000;  // 10 ms
    SysTick->VAL = 0;           // clear
    SysTick->CTRL = 0x03;       // enable

    while (1) {
        __DSB();
        __WFI();
    }
}

static u8 _255_to_200(u8 num) {
    u16 temp = num * 400 / 255;
    temp += temp & 0x01;
    return temp >> 1;
}

void SysTick_Handler(void) {

}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif