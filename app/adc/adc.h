#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

void adc_init();
void adc1_config(uint8_t ADC_SampleTime);
u16 adc2_read();

#endif