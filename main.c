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

static void oscilloscope_init();
static u8 update();
static void roll_mode();
static void trigger_mode();

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
 * B4       EXTI4   RUN/HOLD
 * B5       EXTI5   timebase +
 * B6       EXTI6   timebase -
 * A11      EXTI11  mode +
 * A12      EXTI12  mode -
 * 
 * B7   cp_AC
 * B8   cp_DC
 * B12  vsen_0.1
 * B13  vsen_x5
 * B14  vsen_x2
 * B15  trigger_rise
 *
 * USART1
 * ADC1
 * ADC2
 * SPI1
 * TIM2
 * TIM3
 */

#define BUF_SIZE 1000
static u8 buffer1[BUF_SIZE];
static u8 buffer2[BUF_SIZE];

// Non-existent initial value
static mode_t mode = -1;
static time_base_t time_base = 0;
static status_t status = -1;
static coupling_t coupling = -1;
static v_sen_t v_sen = -1;
static trigger_t trigger = -1;
static u8 trigger_level = 0;

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
    while (1) {
        __DSB();
        __WFI();
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

static void roll_mode() {}

static u16 check_waveform(const u8* data) {
    if (trigger == rising_edge) {
        for (u16 i = 159; i < BUF_SIZE - 159; i++) {
            if (data[i] < trigger_level) {
                for (i++; i < BUF_SIZE - 159; i++) {
                    if (data[i] >= trigger_level) {
                        return i - 160;
                    }
                }
            }
        }
    } else {
        for (u16 i = 159; i < BUF_SIZE - 159; i++) {
            if (data[i] > trigger_level) {
                for (i++; i < BUF_SIZE - 159; i++) {
                    if (data[i] <= trigger_level) {
                        return i - 160;
                    }
                }
            }
        }
    }
    return 0xffff;
}

static void trigger_mode() {
    u8* for_adc = buffer1;
    u8* for_cpu = buffer2;
    adc1_config(time_base);
    tim2_set_freq(time_base);
    dma_set(for_cpu, BUF_SIZE);
    while (dma_finish() == 0)
        ;
    while (1) {
        dma_set(for_adc, BUF_SIZE);
        u16 result = check_waveform(for_cpu);
        if (result != 0xffff) {
            GUI_display_waveform(for_cpu + result);
            if (mode == SINGLE) {
                status = HOLD;
                status_set_status();
                GUI_display_status(status);
            }
        } else if (mode == AUTO) {
            GUI_display_waveform(for_cpu);
        }
        if (update() || mode == ROLL) {
            // stop adc, tim, dma
            return;
        }
        while (dma_finish() == 0)
            ;
        // exchange
        u8* temp = for_adc;
        for_adc = for_cpu;
        for_cpu = temp;
    }
}

void SysTick_Handler(void) {}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    while (1)
        ;
}

#endif