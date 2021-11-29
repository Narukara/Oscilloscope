#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

#include "adc.h"
#include "status.h"

static status_t status = RUN;
status_t status_get_status() {
    return status;
}

/**
 * set status to HOLD
 */
void status_set_status() {
    status = HOLD;
}

static mode_t mode = AUTO;
mode_t status_get_mode() {
    return mode;
}

/**
 * real time
 */
coupling_t status_get_coupling() {
    return AC_coupling;  // stub
}

/**
 * Vertical sensitivity
 * real time
 */
v_sen_t status_get_v_sen() {
    return V1;  // stub
}

/**
 * real time
 */
trigger_t status_get_trigger() {
    return riging_edge;  // stub
}

/**
 * real time
 * @return trigger level 0-255
 */
u8 status_get_trigger_level() {
    return adc2_read() >> 4;
}

static time_base_t time_base = us200;
time_base_t status_get_time_base() {
    return time_base;
}

/**
 * B3 has some weird problems, EXTI cannot be triggered
 * B4       EXTI4   RUN/HOLD
 * B5       EXTI5   timebase +
 * B6       EXTI6   timebase -
 * A11      EXTI11  mode +
 * A12      EXTI12  mode -
 */

void status_init() {
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
                         .GPIO_Mode = GPIO_Mode_IPU,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12,
                         .GPIO_Mode = GPIO_Mode_IPU,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);

    EXTI_Init(&(EXTI_InitTypeDef){
        .EXTI_Line =
            EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line11 | EXTI_Line12,
        .EXTI_Mode = EXTI_Mode_Interrupt,
        .EXTI_Trigger = EXTI_Trigger_Falling,
        .EXTI_LineCmd = ENABLE,
    });
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_Init(&(NVIC_InitTypeDef){
        .NVIC_IRQChannel = EXTI4_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    });
    NVIC_Init(&(NVIC_InitTypeDef){
        .NVIC_IRQChannel = EXTI9_5_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    });
    NVIC_Init(&(NVIC_InitTypeDef){
        .NVIC_IRQChannel = EXTI15_10_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    });
}

void EXTI4_IRQHandler(void) {
    EXTI_ClearFlag(EXTI_Line4);
    if (status == RUN) {
        status = HOLD;
    } else {
        status = RUN;
    }
}

static const time_base_t time_base_list[] = {
    ms500, ms200, ms100, ms50,  ms20,  ms10, ms5,
    ms2,   ms1,   us500, us200, us100, us50, us20,
};

void EXTI9_5_IRQHandler(void) {
    u8 id = (time_base >> 12) & 0x0f;
    if (EXTI_GetITStatus(EXTI_Line5) == SET) {
        EXTI_ClearFlag(EXTI_Line5);
        if (id != 0) {
            time_base = time_base_list[id - 1];
        }
    } else {
        EXTI_ClearFlag(EXTI_Line6);
        if (id != 13) {
            time_base = time_base_list[id + 1];
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line11) == SET) {
        EXTI_ClearFlag(EXTI_Line11);
        if (mode != 0) {
            mode--;
        }
    } else {
        EXTI_ClearFlag(EXTI_Line12);
        if (mode != 3) {
            mode++;
        }
    }
}