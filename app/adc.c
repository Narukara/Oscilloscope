#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define channel ADC_Channel_0

void adc_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_0,
                         .GPIO_Mode = GPIO_Mode_AIN,
                         .GPIO_Speed = GPIO_Speed_50MHz,
                     });

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);  // 56M / 4 = 14M
    ADC_Init(ADC1, &(ADC_InitTypeDef){
                       .ADC_Mode = ADC_Mode_Independent,
                       .ADC_ScanConvMode = DISABLE,
                       .ADC_ContinuousConvMode = DISABLE,
                       .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
                       .ADC_DataAlign = ADC_DataAlign_Right,
                       .ADC_NbrOfChannel = 1,
                   });
    /**
     * Clock = 14MHz
     * 
     * T_conv = Sample Time + 12.5 cycles
     * 
     * Sample Time/cycles      Total/cycles       T/us
     *       1.5                    14             1
     *       7.5                    20             1.43
     *      13.5                    26             1.86
     *      28.5                    41             2.93
     *      41.5                    54             3.86
     *      55.5                    68             4.86
     *      71.5                    84             6
     *     239.5                   252            18
     */
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_71Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
        ;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

u16 adc_read() {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        ;
    return ADC_GetConversionValue(ADC1);
}