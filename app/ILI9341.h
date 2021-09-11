#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f10x.h"

void ILI9341_init();
void addr_set();
void send_color(u8 red, u8 green, u8 blue);

#endif