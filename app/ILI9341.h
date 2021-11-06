#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f10x.h"

void ILI9341_init();
void ILI9341_set_y(u16 begin, u16 end);
void ILI9341_set_x(u16 begin, u16 end);
void ILI9341_set_y_begin(u16 begin);
void ILI9341_set_x_begin(u16 begin);
void ILI9341_begin_write();
void ILI9341_set_pixel(u16 color);
void ILI9341_set_multi_pixel(const u16* color, u8 num);

#endif