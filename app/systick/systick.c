#include "stm32f10x.h"
/**
 * Clock = 56MHz
 * STCLK = 56 / 8 = 7MHz
 */

/**
 * @param us <= 2396745
 */
void systick_delay_us(u32 us) {
    SysTick->LOAD = 7 * us;
    SysTick->VAL = 0;      // clear
    SysTick->CTRL = 0x01;  // enable
    while (!(SysTick->CTRL & 0x10000))
        ;
    SysTick->CTRL = 0;  // disable
}

/**
 * @param us <= 2396745
 */
void systick_set_interrupt_us(u32 us){
    SysTick->LOAD = 7 * us;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x03;
}

void systick_reset_interrupt(){
    SysTick->CTRL = 0;
}