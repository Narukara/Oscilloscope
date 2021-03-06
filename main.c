#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "GUI.h"
#include "ILI9341.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "status.h"
#include "systick.h"
#include "tim.h"
#include "uart.h"

/**
 * ALL Resource
 *
 * GPIO
 * A0       ADC1    IN0
 * A1       ADC2    IN1
 * A2       ILI9341 RESET
 * A3       ILI9341 DC
 * A4       SPI     CS
 * A5       SPI     SCLK
 * A6       SPI     MISO
 * A7       SPI     MOSI
 * A9       UART1   TX
 * A10      UART1   RX
 * B1       TIM3    PWM
 *
 * B3 has some weird problems
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
 *
 * USART1
 * ADC1
 * ADC2
 * SPI1
 * TIM2
 * TIM3
 */

// Non-existent initial value
static mode_t mode = -1;
static time_base_t time_base = 0;
static status_t status = -1;
static coupling_t coupling = -1;
static v_sen_t v_sen = -1;
static trigger_t trigger = -1;
static u8 trigger_level = 0;  // this may happen, but it's not a serious problem

static void oscilloscope_init();
static u8 update();
static void roll_mode();
static void trigger_mode();

int main() {
    SCB->CCR |=
        SCB_CCR_STKALIGN_Msk;  // set STKALIGN, for Cortex-M3 r1p0 and r1p1
    oscilloscope_init();
    update();
    while (1) {
        if (mode != ROLL) {
            trigger_mode();
        } else {
            roll_mode();
        }
    }
}

static void oscilloscope_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                               RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
                           ENABLE);
    tim_init();
    uart_init();
    status_init();
    adc_init();
    dma_init();
    spi_init();
    ILI9341_init();
    GUI_init();
}

/**
 * @return 1 if time_base changed
 */
static u8 update() {
    /**
     * Mode and time_base may conflict with each other. Therefore, interrupts
     * need to be disabled when updating.
     */
    u8 ret = 0;
    __disable_irq();
    mode_t mode_new = status_get_mode();
    if (mode != mode_new) {
        mode = mode_new;
        GUI_display_mode(mode);
    }
    time_base_t time_base_new = status_get_time_base();
    if (time_base != time_base_new) {
        time_base = time_base_new;
        ret = 1;
        GUI_display_time_base(time_base);
    }
    __enable_irq();

    status_t status_new = status_get_status();
    if (status != status_new) {
        status = status_new;
        GUI_display_status(status);
    }

    trigger_t trigger_new = status_get_trigger();
    if (trigger != trigger_new) {
        trigger = trigger_new;
        GUI_display_trigger(trigger);
    }

    u8 trigger_level_new = status_get_trigger_level();
    if (trigger_level != trigger_level_new) {
        trigger_level = trigger_level_new;
        GUI_display_trigger_level(trigger_level);
    }

    v_sen_t v_sen_new = status_get_v_sen();
    if (v_sen != v_sen_new) {
        v_sen = v_sen_new;
        GUI_display_v_sen(v_sen);
    }

    coupling_t coupling_new = status_get_coupling();
    if (coupling != coupling_new) {
        coupling = coupling_new;
        GUI_display_coupling(coupling);
    }
    return ret;
}

/******* ROLL MODE *******/

static const u16 roll_Time[] = {25000, 10000, 5000, 2500};  // us
static volatile u8 roll_start = 0;

static void roll_mode() {
    adc1_config(time_base);
    u8 id = (time_base >> 12) & 0x0f;
    systick_set_interrupt_us(roll_Time[id]);
    u16 roll_t = 0;
    while (1) {
        while (roll_start == 0) {
            __DSB();
            __WFE();
        }
        roll_start = 0;
        GUI_display_waveform_point(adc1_read(), roll_t++);
        if (roll_t == 320) {
            roll_t = 0;
        }
        if (update() || mode != ROLL) {
            systick_reset_interrupt();
            adc1_disable();
            return;
        }
    }
}

void SysTick_Handler() {
    roll_start = 1;
}

/******* TRIGGER MODE *******/
/** include AUTO, NORMAL, SIGNLE **/

#define BUF_SIZE 1000
static u8 buffer1[BUF_SIZE];
static u8 buffer2[BUF_SIZE];

/**
 * @param data
 * @brief check whether there is a waveform that meets the trigger conditions
 * @return index of trigger point. if tigger fails, return 0xffff
 */
static u16 check_waveform(const u8* data) {
    if (trigger == rising_edge) {
        for (u16 i = 159; i < BUF_SIZE - 159; i++) {
            if (data[i] < trigger_level) {
                for (i++; i < BUF_SIZE - 159; i++) {
                    if (data[i] > trigger_level) {
                        return i - 160;
                    }
                }
            }
        }
    } else {
        for (u16 i = 159; i < BUF_SIZE - 159; i++) {
            if (data[i] > trigger_level) {
                for (i++; i < BUF_SIZE - 159; i++) {
                    if (data[i] < trigger_level) {
                        return i - 160;
                    }
                }
            }
        }
    }
    return 0xffff;
}

static void trigger_mode() {
    u8* buf_adc = buffer1;
    u8* buf_cpu = buffer2;
    adc1_config(time_base);
    tim2_set_freq(time_base);
    dma_set(buf_cpu, BUF_SIZE);
    while (dma_finish() == 0) {
        __DSB();
        __WFE();
    }
    while (1) {
        dma_set(buf_adc, BUF_SIZE);
        u16 result = check_waveform(buf_cpu);
        if (result != 0xffff) {
            GUI_display_waveform(buf_cpu + result);
            if (mode == SINGLE) {
                status = HOLD;
                status_set_status();
                GUI_display_status(status);
            }
        } else if (mode == AUTO) {
            GUI_display_waveform(buf_cpu);
        }
        if (update() || mode == ROLL) {
            tim2_disable();
            adc1_disable();
            dma_disable();
            return;
        }
        while (dma_finish() == 0) {
            __DSB();
            __WFE();
        }
        // exchange
        u8* temp = buf_adc;
        buf_adc = buf_cpu;
        buf_cpu = temp;
    }
}

#ifdef USE_FULL_ASSERT

#define UNUSED(X) (void)X  // To avoid gcc warnings

void assert_failed(uint8_t* file, uint32_t line) {
    UNUSED(file);
    UNUSED(line);
    while (1)
        ;
}

#endif
