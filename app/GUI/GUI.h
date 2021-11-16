#ifndef GUI_H
#define GUI_H

#include "stm32f10x.h"

#include "status.h"

void GUI_init();
void GUI_display_waveform(const u8* data);
void GUI_display_v_sen(v_sen_t v_sen);
void GUI_display_coupling(coupling_t coupling);
void GUI_display_time_base(time_base_t time_base);
void GUI_display_mode(mode_t mode);
void GUI_display_trigger(trigger_t trigger);
void GUI_display_trigger_level(u8 trigger_level);
void GUI_display_status(status_t status);

#endif