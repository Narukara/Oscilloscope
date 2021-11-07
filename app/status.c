#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"

#include "status.h"

static status_t status = HOLD;
status_t status_get_status() {
    return status;
}

static mode_t mode = ROLL;
mode_t status_get_mode() {
    return mode;
}

/**
 * real time
 */
coupling_t status_get_coupling_method() {
    return GND_coupling;  // stub
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
    return 127;  // stub
}

static time_base_t time_base = ms200;
time_base_t status_get_time_base() {
    return time_base;
}
