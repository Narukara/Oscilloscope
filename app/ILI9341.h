#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f10x.h"

void ILI9341_init();
void commit_display();
void set_pixel(u8 y, u16 color);

#endif