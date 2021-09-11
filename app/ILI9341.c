#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "spi.h"
#include "systick.h"

// GPIOA
#define CS GPIO_Pin_4
#define DC GPIO_Pin_8
// GPIOB
#define RESET GPIO_Pin_0

/**
 * SPI1
 * PA4  CS
 * PA5  SCLK
 * PA6  MISO
 * PA7  MOSI
 */

/**
 * PA8 D/C
 * 1 data
 * 0 command
 */

static void send_via_spi(u8 data, BitAction dc) {
    GPIO_WriteBit(GPIOA, CS, Bit_RESET);
    GPIO_WriteBit(GPIOA, DC, dc);
    spi_send(data);
    GPIO_WriteBit(GPIOA, CS, Bit_SET);
}

#define send_command(cmd) send_via_spi(cmd, Bit_RESET)
#define send_data(data) send_via_spi(data, Bit_SET)

/**
 * red:     0-31
 * green:   0-63
 * blue:    0-31
 */
void send_color(u8 red, u8 green, u8 blue) {
    send_data((red << 3) | (green >> 3));
    send_data((green << 5) | blue);
}

void ILI9341_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_WriteBit(GPIOB, RESET, Bit_RESET);
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = DC,
                         .GPIO_Mode = GPIO_Mode_Out_PP,
                         .GPIO_Speed = GPIO_Speed_50MHz,
                     });
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                         .GPIO_Pin = RESET,
                         .GPIO_Mode = GPIO_Mode_Out_PP,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });
    systick_delay_us(10000);
    GPIO_WriteBit(GPIOB, RESET, Bit_SET);
    systick_delay_us(50000);
    send_command(0x11);
    send_data(0x00);

    send_command(0xCF);
    send_data(0X00);
    send_data(0XC1);
    send_data(0X30);

    send_command(0xED);
    send_data(0X64);
    send_data(0X03);
    send_data(0X12);
    send_data(0X81);

    send_command(0xE8);
    send_data(0X85);
    send_data(0X11);
    send_data(0X78);

    send_command(0xF6);
    send_data(0X01);
    send_data(0X30);
    send_data(0X00);

    send_command(0xCB);
    send_data(0X39);
    send_data(0X2C);
    send_data(0X00);
    send_data(0X34);
    send_data(0X05);

    send_command(0xF7);
    send_data(0X20);

    send_command(0xEA);
    send_data(0X00);
    send_data(0X00);

    send_command(0xC0);
    send_data(0X20);

    send_command(0xC1);
    send_data(0X11);

    send_command(0xC5);
    send_data(0X31);
    send_data(0X3C);

    send_command(0xC7);
    send_data(0XA9);

    send_command(0x3A);
    send_data(0X55);

    send_command(0x36);
    send_data(0x48);  //竖屏参数

    send_command(0xB1);
    send_data(0X00);
    send_data(0X18);

    send_command(0xB4);
    send_data(0X00);
    send_data(0X00);

    send_command(0xF2);
    send_data(0X00);

    send_command(0x26);
    send_data(0X01);

    send_command(0xE0);
    send_data(0X0F);
    send_data(0X17);
    send_data(0X14);
    send_data(0X09);
    send_data(0X0C);
    send_data(0X06);
    send_data(0X43);
    send_data(0X75);
    send_data(0X36);
    send_data(0X08);
    send_data(0X13);
    send_data(0X05);
    send_data(0X10);
    send_data(0X0B);
    send_data(0X08);

    send_command(0xE1);
    send_data(0X00);
    send_data(0X1F);
    send_data(0X23);
    send_data(0X03);
    send_data(0X0E);
    send_data(0X04);
    send_data(0X39);
    send_data(0X25);
    send_data(0X4D);
    send_data(0X06);
    send_data(0X0D);
    send_data(0X0B);
    send_data(0X33);
    send_data(0X37);
    send_data(0X0F);

    send_command(0x29);
}

void addr_set() {
    send_command(0x2a);
    send_data(0x00);
    send_data(0x00);
    send_command(0x2b);
    send_data(0x00);
    send_data(0x00);
    send_command(0x2c);
}