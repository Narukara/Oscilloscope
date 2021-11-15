#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"

void spi_init();
void spi_send(u8 data);

#endif