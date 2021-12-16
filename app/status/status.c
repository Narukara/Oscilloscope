#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

#include "adc.h"
#include "status.h"

/**
 * In pull up
 *
 * B7   cp_AC
 * B8   cp_DC
 *
 * B12  vsen_0.1
 * B13  vsen_x5
 * B14  vsen_x2
 *
 * B15  trigger_rise
 */

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
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0) {
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
 * B12  vsen_0.1
 * B13  vsen_x5
 * B14  vsen_x2
 */
v_sen_t status_get_v_sen() {
    v_sen_t vsen;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0) {
        vsen = 5;
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) {
        vsen = 2;
    } else {
        vsen = 1;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) != 0) {
        vsen <<= 4;
    }
    return vsen;
}

/**
 * real time
 */
trigger_t status_get_trigger() {
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) {
        return rising_edge;
    } else {
        return falling_edge;
    }
}

/**
 * real time
 * @return trigger level 0-255
 */
u8 status_get_trigger_level() {
    return 255 - (adc2_read() >> 4);
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
                         .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |
                                     GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_12 |
                                     GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
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

void EXTI4_IRQHandler(void) {
    EXTI_ClearFlag(EXTI_Line4);
    simple_delay();
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0) {
        if (status == RUN) {
            status = HOLD;
        } else {
            status = RUN;
        }
    }
}

static const time_base_t time_base_list[] = {
    ms500, ms200, ms100, ms50,  ms20,  ms10, ms5,
    ms2,   ms1,   us500, us200, us100, us50, us20,
};

void EXTI9_5_IRQHandler(void) {
    u8 id = (time_base >> 12) & 0x0f;
    simple_delay();
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
    } else {
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
    }
}

void EXTI15_10_IRQHandler(void) {
    simple_delay();
    if (EXTI_GetITStatus(EXTI_Line11) == SET) {
        EXTI_ClearFlag(EXTI_Line11);
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) {
            if (mode != 0) {
                mode--;
            }
        }
    } else {
        EXTI_ClearFlag(EXTI_Line12);
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0) {
            if (mode != 3) {
                mode++;
            }
        }
    }
    u8 id = (time_base >> 12) & 0x0f;
    if (mode != ROLL) {
        if (id <= 3) {
            time_base = us200;
        }
    } else {
        if (id > 3) {
            time_base = ms50;
        }
    }
}