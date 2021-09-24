#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

/**
 * SPI1
 * PA4  CS
 * PA5  SCLK
 * PA6  MISO
 * PA7  MOSI
 */

void spi_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_4,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_Out_PP});
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_AF_PP});
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){.GPIO_Pin = GPIO_Pin_6,
                                         .GPIO_Speed = GPIO_Speed_50MHz,
                                         .GPIO_Mode = GPIO_Mode_IN_FLOATING});

    SPI_Init(SPI1, &(SPI_InitTypeDef){
                       .SPI_BaudRatePrescaler =
                           SPI_BaudRatePrescaler_4,  // 56 / 4 = 14MHz < 18MHz
                       .SPI_CPOL = 0,
                       .SPI_CPHA = 0,
                       .SPI_DataSize = SPI_DataSize_8b,
                       .SPI_Direction = SPI_Direction_2Lines_FullDuplex,
                       .SPI_FirstBit = SPI_FirstBit_MSB,
                       .SPI_Mode = SPI_Mode_Master,
                       .SPI_NSS = SPI_NSS_Soft,
                       .SPI_CRCPolynomial = 7});
    SPI_Cmd(SPI1, ENABLE);
}

void spi_send(u8 data) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;
    SPI_I2S_SendData(SPI1, data);
}