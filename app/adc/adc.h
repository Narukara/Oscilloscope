#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

#include "status.h"

void adc_init();
void adc1_config(time_base_t time_base);
u16 adc2_read();

#endif