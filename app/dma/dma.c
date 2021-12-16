#include "misc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"

// DMA -> DISABLE
void dma_init() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_Init(DMA1_Channel1,
             &(DMA_InitTypeDef){
                 .DMA_PeripheralBaseAddr = (u32)0x4001244D,  // ADC1->DR + 1
                 .DMA_MemoryBaseAddr = 0,
                 .DMA_DIR = DMA_DIR_PeripheralSRC,
                 .DMA_BufferSize = 0,
                 .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
                 .DMA_MemoryInc = DMA_MemoryInc_Enable,
                 .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
                 .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
                 .DMA_Mode = DMA_Mode_Normal,
                 .DMA_Priority = DMA_Priority_High,
                 .DMA_M2M = DMA_M2M_Disable,
             });
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    NVIC_Init(&(NVIC_InitTypeDef){
        .NVIC_IRQChannel = DMA1_Channel1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    });
    DMA_Cmd(DMA1_Channel1, DISABLE);
}

// DMA -> ENABLE
void dma_set(const u8* buffer, u16 buffer_size) {
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA1_Channel1->CMAR = (u32)buffer;
    DMA1_Channel1->CNDTR = buffer_size;
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void dma_disable() {
    DMA_Cmd(DMA1_Channel1, DISABLE);
}

/**
 * @return 1 if finish
 */
u8 dma_finish() {
    if (DMA_GetCurrDataCounter(DMA1_Channel1) == 0) {
        return 1;
    }
    return 0;
}

void DMA1_Channel1_IRQHandler() {
    DMA_ClearFlag(DMA1_FLAG_TC1);
}