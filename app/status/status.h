#ifndef STATUS_H
#define STATUS_H

#include "stm32f10x.h"

// size of enum = 4

typedef enum {
    V0_1 = 0x01,
    V0_2 = 0x02,
    V0_5 = 0x05,
    V1 = 0x10,
    V2 = 0x20,
    V5 = 0x50,
} v_sen_t;

typedef enum {
    GND_coupling,
    DC_coupling,
    AC_coupling,
} coupling_t;

typedef enum {
    riging_edge,
    falling_edge,
} trigger_t;

typedef enum {
    /**
     * 2 bytes       | half byte | half byte      | 1 byte
     * TIM Prescaler | id        | ADC SampleTime | GUI
     */
    ms500 = ((50000 - 1) << 16) + (0  << 12) + (7 << 8) + 0x55,
    ms200 = ((20000 - 1) << 16) + (1  << 12) + (7 << 8) + 0x52,
    ms100 = ((10000 - 1) << 16) + (2  << 12) + (7 << 8) + 0x51,
    ms50  = ((5000  - 1) << 16) + (3  << 12) + (7 << 8) + 0x35,
    ms20  = ((2000  - 1) << 16) + (4  << 12) + (7 << 8) + 0x32,
    ms10  = ((1000  - 1) << 16) + (5  << 12) + (7 << 8) + 0x31,
    ms5   = ((500   - 1) << 16) + (6  << 12) + (7 << 8) + 0x15,
    ms2   = ((200   - 1) << 16) + (7  << 12) + (7 << 8) + 0x12,
    ms1   = ((100   - 1) << 16) + (8  << 12) + (7 << 8) + 0x11,
    us500 = ((50    - 1) << 16) + (9  << 12) + (7 << 8) + 0x45,
    us200 = ((20    - 1) << 16) + (10 << 12) + (6 << 8) + 0x42,
    us100 = ((10    - 1) << 16) + (11 << 12) + (4 << 8) + 0x41,
    us50  = ((5     - 1) << 16) + (12 << 12) + (2 << 8) + 0x25,
    us20  = ((2     - 1) << 16) + (13 << 12) + (0 << 8) + 0x22,
} time_base_t;

typedef enum {
    RUN,
    HOLD,
} status_t;

typedef enum {
    ROLL,
    SINGLE,
    NORMAL,
    AUTO,
} mode_t;

status_t status_get_status();
void status_set_status();
mode_t status_get_mode();
coupling_t status_get_coupling();
v_sen_t status_get_v_sen();
trigger_t status_get_trigger();
u8 status_get_trigger_level();
time_base_t status_get_time_base();
void status_init();

#endif