#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_usart.h"

#include "ILI9341.h"
#include "spi.h"
#include "systick.h"
#include "uart.h"

int main() {
    SCB->CCR |=
        SCB_CCR_STKALIGN_Msk;  // set STKALIGN, for Cortex-M3 r1p0 and r1p1

    uart_init();
    uart_send("Hi");
    spi_init();
    ILI9341_init();
    addr_set();
    for (u16 i = 0; i < 320; i++) {
        for (u8 j = 0; j < 240; j++) {
            send_color(i / 10 % 32, 0, j / 8 % 32);
        }
    }
    while (1) {
        __DSB();
        __WFI();
    }
}

void SysTick_Handler(void) {
    ;
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif