#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

#include "adc.h"
#include "status.h"

u8 status_255_to_200(u8 num) {
    u16 temp = num * 400 / 255;
    temp += temp & 0x01;
    return temp >> 1;
}

static status_t status = RUN;
status_t status_get_status() {
    return status;
}

static mode_t mode = NORMAL;
mode_t status_get_mode() {
    return mode;
}

/**
 * real time
 */
coupling_t status_get_coupling_method() {
    return DC_coupling;  // stub
}

/**
 * Vertical sensitivity
 * real time
 */
v_sen_t status_get_v_sen() {
    return V0_5;  // stub
}

/**
 * real time
 */
trigger_t status_get_trigger_mode() {
    return falling_edge;  // stub
}

/**
 * real time
 * @return trigger level 0-255
 */
u8 status_get_trigger_level() {
    return adc2_read() >> 4;
}

static time_base_t time_base = ms20;
time_base_t status_get_time_base() {
    return time_base;
}
