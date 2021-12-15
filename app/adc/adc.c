#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "status.h"

/**
 * ADC1 -> DISABLE
 * Continue to configure ADC1 with adc1_config
 */
void adc_init() {
    // PA0, PA1
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1,
                         .GPIO_Mode = GPIO_Mode_AIN,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    // ADCCLK
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);  // 56M / 4 = 14M
    // PA1 -> ADC2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    ADC_Init(ADC2, &(ADC_InitTypeDef){
                       .ADC_Mode = ADC_Mode_Independent,
                       .ADC_ScanConvMode = DISABLE,
                       .ADC_ContinuousConvMode = DISABLE,
                       .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
                       .ADC_DataAlign = ADC_DataAlign_Right,
                       .ADC_NbrOfChannel = 1,
                   });
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1,
                             ADC_SampleTime_71Cycles5);  // adjustable
    ADC_Cmd(ADC2, ENABLE);
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2) == SET)
        ;
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2) == SET)
        ;
    // PA0 -> ADC1, TIM2 -> ADC1 -> DMA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_Init(ADC1, &(ADC_InitTypeDef){
                       .ADC_Mode = ADC_Mode_Independent,
                       .ADC_ScanConvMode = DISABLE,
                       .ADC_ContinuousConvMode = DISABLE,
                       .ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2,
                       .ADC_DataAlign = ADC_DataAlign_Left,
                       .ADC_NbrOfChannel = 1,
                   });
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_71Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET)
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET)
        ;

    ADC_Cmd(ADC1, DISABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

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

/**
 * ADC1 -> ENABLE
 */
void adc1_config(time_base_t time_base) {
    ADC_Cmd(ADC1, DISABLE);
    u8 id = (time_base >> 12) & 0x0f;
    ADC_Init(ADC1, &(ADC_InitTypeDef){
                       .ADC_Mode = ADC_Mode_Independent,
                       .ADC_ScanConvMode = DISABLE,
                       .ADC_ContinuousConvMode = (id != 13) ? DISABLE : ENABLE,
                       .ADC_ExternalTrigConv = (id > 3 && id < 13)
                                                   ? ADC_ExternalTrigConv_T2_CC2
                                                   : ADC_ExternalTrigConv_None,
                       .ADC_DataAlign = ADC_DataAlign_Left,
                       .ADC_NbrOfChannel = 1,
                   });
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,
                             (u8)((time_base >> 8) & 0x0f));
    ADC_Cmd(ADC1, ENABLE);
    if (id == 13) {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    }
}

void adc1_disable() {
    ADC_Cmd(ADC1, DISABLE);
}

u8 adc1_read() {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        ;
    return *((volatile u8*)0x4001244D);
}

/**
 * @return align right
 */
u16 adc2_read() {
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
    while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
        ;
    return ADC_GetConversionValue(ADC2);
}