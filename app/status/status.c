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

static time_base_t time_base = us200;
time_base_t status_get_time_base() {
    return time_base;
}

/**
 * real time
 * @return trigger level 0-255
 */
u8 status_get_trigger_level() {
    return 255 - (adc2_read() >> 4);
}

/**
 * Coupling method
 * real time
 *
 * B8   cp_DC
 * B9   cp_AC
 */
coupling_t status_get_coupling() {
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0) {
        return AC_coupling;
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0) {
        return DC_coupling;
    } else {
        return GND_coupling;
    }
}

/**
 * Vertical sensitivity
 * real time
 *
 * B12  vsen_x5
 * B13  vsen_x2
 * B14  vsen_0.1
 */
v_sen_t status_get_v_sen() {
    v_sen_t vsen;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) {
        vsen = 5;
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0) {
        vsen = 2;
    } else {
        vsen = 1;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) != 0) {
        vsen <<= 4;
    }
    return vsen;
}

/**
 * real time
 *
 * B15  trigger_rise
 */
trigger_t status_get_trigger() {
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) {
        return rising_edge;
    } else {
        return falling_edge;
    }
}

/**
 * B3 has some weird problems, EXTI cannot be triggered
 * A11, A12         USB
 * B4       EXTI4   mode -
 * B5       EXTI5   timebase +
 * B6       EXTI6   timebase -
 * B7       EXTI7   RUN/HOLD
 * A15      EXTI15  mode +
 *
 * B8   cp_DC
 * B9   cp_AC
 * B12  vsen_x5
 * B13  vsen_x2
 * B14  vsen_0.1
 * B15  trigger_rise
 */
void status_init() {
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |
                                     GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                     GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                     GPIO_Pin_15,
                         .GPIO_Mode = GPIO_Mode_IPU,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = GPIO_Pin_15,
                         .GPIO_Mode = GPIO_Mode_IPU,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);

    EXTI_Init(&(EXTI_InitTypeDef){
        .EXTI_Line =
            EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line15,
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

#pragma GCC push_options
#pragma GCC optimize("O0")
/**
 * switch bounce
 */
static void simple_delay() {
    for (u32 i = 0; i < 20000; i++)
        ;
}
#pragma GCC pop_options

/**
 * B4       EXTI4   mode -
 * B5       EXTI5   timebase +
 * B6       EXTI6   timebase -
 * B7       EXTI7   RUN/HOLD
 * A15      EXTI15  mode +
 */

static const time_base_t time_base_list[] = {
    ms500, ms200, ms100, ms50,  ms20,  ms10, ms5,
    ms2,   ms1,   us500, us200, us100, us50, us20,
};

void EXTI4_IRQHandler(void) {
    simple_delay();
    EXTI_ClearFlag(EXTI_Line4);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0) {
        if (mode == ROLL) {
            time_base = us200;
        }
        if (mode != 3) {
            mode++;
        }
    }
}

void EXTI9_5_IRQHandler(void) {
    simple_delay();
    u8 id = (time_base >> 12) & 0x0f;
    if (EXTI_GetITStatus(EXTI_Line5) == SET) {
        EXTI_ClearFlag(EXTI_Line5);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0) {
            if (mode != ROLL) {
                if (id != 4) {
                    time_base = time_base_list[id - 1];
                }
            } else {
                if (id != 0) {
                    time_base = time_base_list[id - 1];
                }
            }
        }
    } else if (EXTI_GetITStatus(EXTI_Line6) == SET) {
        EXTI_ClearFlag(EXTI_Line6);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {
            if (mode != ROLL) {
                if (id != 13) {
                    time_base = time_base_list[id + 1];
                }
            } else {
                if (id != 3) {
                    time_base = time_base_list[id + 1];
                }
            }
        }
    } else {
        EXTI_ClearFlag(EXTI_Line7);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0) {
            if (status == RUN) {
                status = HOLD;
            } else {
                status = RUN;
            }
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    simple_delay();
    EXTI_ClearFlag(EXTI_Line15);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0) {
        if (mode != 0) {
            mode--;
        }
        if (mode == ROLL) {
            time_base = ms50;
        }
    }
}