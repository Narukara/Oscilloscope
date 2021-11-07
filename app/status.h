#ifndef STATUS_H
#define STATUS_H

typedef enum v_sen_t {
    V0_1 = 1,
    V0_2 = 2,
    V0_5 = 5,
    V1 = 8,
    V2 = 16,
    V5 = 40,
} v_sen_t;

typedef enum coupling_t {
    GND_coupling,
    DC_coupling,
    AC_coupling,
} coupling_t;

typedef enum trigger_t {
    riging_edge,
    falling_edge,
} trigger_t;

typedef enum time_base_t {
    ms500 = 0x55,
    ms200 = 0x25,
    ms100 = 0x15,
    ms50 = 0x53,
    ms20 = 0x23,
    ms10 = 0x13,
    ms5 = 0x51,
    ms2 = 0x21,
    ms1 = 0x11,
    us500 = 0x54,
    us200 = 0x24,
    us100 = 0x14,
    us50 = 0x52,
    us20 = 0x22,
} time_base_t;

typedef enum status_t {
    RUN,
    HOLD,
} status_t;

typedef enum mode_t {
    ROLL,
    SINGLE,
    NORMAL,
    AUTO,
} mode_t;

status_t status_get_status();
mode_t status_get_mode();
coupling_t status_get_coupling_method();
v_sen_t status_get_v_sen();
trigger_t status_get_trigger_mode();
u8 status_get_trigger_level();
time_base_t status_get_time_base();

#endif