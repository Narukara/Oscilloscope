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
void status_set_status(){
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

static time_base_t time_base = us100;
time_base_t status_get_time_base() {
    return time_base;
}
