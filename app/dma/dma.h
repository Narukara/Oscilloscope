#ifndef DMA_H
#define DMA_H

#include "stm32f10x.h"

void dma_init();
void dma_set(const u8* buffer, u16 buffer_size);
u8 dma_finish();

#endif