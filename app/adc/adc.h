#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

#include "status.h"

void adc_init();
void adc1_config(time_base_t time_base);
void adc1_disable();
u8 adc1_read();
u16 adc2_read();

#endif