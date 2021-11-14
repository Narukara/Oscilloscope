#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "status.h"

// SYSCLK = 56MHz

/**
 * @brief mainly init TIM3
 * TIM2 -> DISABLE
 * Continue to configure TIM2 with tim2_set_freq
 */
void tim_init() {
    // TIM2 -> ADC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // PB1
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_1,
                         .GPIO_Mode = GPIO_Mode_AF_PP,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    // TIM3 -> 1kHz PWM
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInit(TIM3, &(TIM_TimeBaseInitTypeDef){
                               .TIM_Period = 2 - 1,
                               .TIM_Prescaler = 28000 - 1,
                               .TIM_ClockDivision = TIM_CKD_DIV1,
                               .TIM_CounterMode = TIM_CounterMode_Up,
                           });
    TIM_OC4Init(TIM3, &(TIM_OCInitTypeDef){
                          .TIM_OCMode = TIM_OCMode_PWM1,
                          .TIM_OutputState = TIM_OutputState_Enable,
                          .TIM_Pulse = 1,
                          .TIM_OCPolarity = TIM_OCPolarity_Low,
                      });
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * TIM2 -> ENABLE
 */
void tim2_set_freq(time_base_t time_base) {
    TIM_Cmd(TIM2, DISABLE);
    // TODO
    TIM_TimeBaseInit(TIM2, &(TIM_TimeBaseInitTypeDef){
                               .TIM_Period = 2800 - 1,
                               .TIM_Prescaler = 200 - 1,
                               .TIM_ClockDivision = TIM_CKD_DIV1,
                               .TIM_CounterMode = TIM_CounterMode_Up,
                           });
    TIM_OC2Init(TIM2, &(TIM_OCInitTypeDef){
                          .TIM_OCMode = TIM_OCMode_PWM1,
                          .TIM_OutputState = TIM_OutputState_Enable,
                          .TIM_Pulse = 1400,
                          .TIM_OCPolarity = TIM_OCPolarity_Low,
                      });
    TIM_Cmd(TIM2, ENABLE);
}