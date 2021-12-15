#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f10x.h"

void systick_delay_us(u32 us);
/**
 * @param ms <= 2396
 */
#define systick_delay_ms(ms) systick_delay_us((ms)*1000)

void systick_set_interrupt_us(u32 us);
/**
 * @param ms <= 2396
 */
#define systick_set_interrupt_ms(ms) systick_set_interrupt_us((ms)*1000)
void systick_reset_interrupt();

#endif