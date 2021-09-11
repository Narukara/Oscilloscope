#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_usart.h"

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
    for (u16 i = 0; i < 320; i++) {
        commit_display();
    }
    SysTick->LOAD = 9 * 10000;  // 10 ms
    SysTick->VAL = 0;           // clear
    SysTick->CTRL = 0x03;       // enable

    while (1) {
        __DSB();
        __WFI();
    }
}

static u8 _255_to_239(u8 num) {
    u16 temp = num * 478 / 255;
    temp += temp & 0x01;
    return temp >> 1;
}

void SysTick_Handler(void) {
    set_pixel(_255_to_239(adc_read() >> 4), 0x001f);
    commit_display();
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif