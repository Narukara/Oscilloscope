#include "stm32f10x.h"

/**
 * @param us <= 1864135
 */
void systick_delay_us(u32 us) {
    SysTick->LOAD = 9 * us;
    SysTick->VAL = 0;      // clear
    SysTick->CTRL = 0x03;  // enable
    while (!(SysTick->CTRL & 0x10000)) {
        __DSB();
        __WFE();
    }
    SysTick->CTRL = 0;  // disable
}

void SysTick_Handler(void) {
    ;
}
