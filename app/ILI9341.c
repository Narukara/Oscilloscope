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

#define send_command(cmd) send_via_spi((cmd), Bit_RESET)
#define send_data(data) send_via_spi((data), Bit_SET)

/**
 *    y
 *    ^
 * 239|
 *    |
 *    |
 *    |
 *    |
 *    0-------------------------> x
 *                            319
 */
static u16 display_ram[240];
static u16 x = 0;

/**
 * @param y 0 <= y <= 239
 * @param color
 *        red    green    blue
 * bits   5       6       5
 */
void set_pixel(u8 y, u16 color) {
    display_ram[y] = color;
}

static void set_background() {
    u32* p = display_ram;
    if (x != 0 && x != 160 && x != 319) {
        // all black
        for (u8 i = 0; i < 120; i++) {
            p[i] = 0;
        }
        display_ram[120] = 0xffff;
        if (x % 10 != 0) {
            display_ram[0] = display_ram[239] = 0xffff;
        } else {
            p[0] = p[1] = p[2] = p[119] = p[118] = p[117] = 0xffffffff;
        }
        if (x < 6 || x > 313) {
            for (u8 y = 10; y < 239; y += 10) {
                display_ram[y] = 0xffff;
            }
        }
    } else {
        // y = 0 or 160 or 319, all white
        for (u8 i = 0; i < 120; i++) {
            p[i] = 0xffffffff;
        }
    }
}

void commit_display() {
    for (u8 y = 0; y < 240; y++) {
        send_data((u8)(display_ram[y] >> 8));
        send_data((u8)display_ram[y]);
    }
    x++;
    if (x == 320) {
        x = 0;
    }
    set_background();
}

void ILI9341_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

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

    GPIO_WriteBit(GPIOB, RESET, Bit_RESET);
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
    send_data(0x48);

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

    // set addr
    send_command(0x2a);
    send_data(0x00);
    send_data(0x00);
    send_command(0x2b);
    send_data(0x00);
    send_data(0x00);
    send_command(0x2c);

    set_background();
}
