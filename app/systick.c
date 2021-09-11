#include "stm32f10x.h"

/**
 * @param us <= 1864135
 */
void systick_delay_us(u32 us) {
    SysTick->LOAD = 9 * us;
    SysTick->VAL = 0;      // clear
    SysTick->CTRL = 0x01;  // enable
    while (!(SysTick->CTRL & 0x10000))
        ;
    SysTick->CTRL = 0;  // disable
}
