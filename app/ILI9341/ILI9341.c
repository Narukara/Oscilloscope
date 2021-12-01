#include "stm32f10x_gpio.h"

#include "spi.h"
#include "systick.h"

// GPIOA
#define RESET GPIO_Pin_2
#define DC GPIO_Pin_3
#define CS GPIO_Pin_4

/**
 * PA3 D/C
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

static void send_data_16(u16 data) {
    send_data((u8)(data >> 8));
    send_data((u8)data);
}

/**
 *     (0,0)       Page (x)
 *        ----------------------> 319 (0x13F,0)
 *        | |    /|    /|       |
 *        | |   / |   / |       |
 * Column | |  /  |  /  |       | (Pins)
 *  (y)   | | /   | /   |       |
 *        | v     v     v       |
 *        v----------------------
 *       239
 *     (0,0xEF)
 */

/**
 * Usage:
 * 0. ILI9341_init
 * 1. ILI9341_set_y
 * 2. ILI9341_set_x
 * 3. ILI9341_begin_write
 * 4. ILI9341_set_pixel / multi_pixel
 * 5. goto step 4 or step 1
 */

/**
 * @brief set column range: [begin, end]
 * @param begin begin <= end
 * @param end end <= 0xEF
 */
void ILI9341_set_y(u16 begin, u16 end) {
    send_command(0x2a);
    send_data_16(begin);
    send_data_16(end);
}

/**
 * @brief set column begin addr only
 * @param begin begin <= end
 */
void ILI9341_set_y_begin(u16 begin) {
    send_command(0x2a);
    send_data_16(begin);
}

/**
 * @brief set page range: [begin, end]
 * @param begin begin <= end
 * @param end end <= 0x13F
 */
void ILI9341_set_x(u16 begin, u16 end) {
    send_command(0x2b);
    send_data_16(begin);
    send_data_16(end);
}

/**
 * @brief set page begin addr only
 * @param begin begin <= end
 */
void ILI9341_set_x_begin(u16 begin) {
    send_command(0x2b);
    send_data_16(begin);
}

/**
 * Call this function ONCE before set_pixel or set_multi_pixel
 * This "write state" will remain until another command is sent
 */
void ILI9341_begin_write() {
    send_command(0x2c);
}

/**
 * @brief Remember to call ILI9341_begin_write() first
 * @param color
 *        red    green    blue
 * bits   5       6       5
 */
void ILI9341_set_pixel(u16 color) {
    send_data_16(color);
}

/**
 * @brief Remember to call ILI9341_begin_write() first
 * @param color pointer to pixel(color) array
 * @param num   number of pixels
 *        red    green    blue
 * bits   5       6       5
 */
void ILI9341_set_multi_pixel(const u16* color, u8 num) {
    for (u8 i = 0; i < num; i++) {
        send_data_16(color[i]);
    }
}

/**
 * Something useful:
 *
 * Power Levels:
 * 1. Normal Mode On (full display), Idle Mode Off, Sleep Out.
 *
 * Normal Display ON or Partial Mode ON, Vertical Scroll Mode OFF --- dot to dot
 *
 * Vertical Scroll Mode: There is a vertical scrolling mode, which is determined
 * by the commands “Vertical Scrolling Definition” (33h) and “Vertical Scrolling
 * Start Address” (37h).
 *
 * The data is written in the order illustrated above. The Counter which
 * dictates where in the physical memory the data is to be written is controlled
 * by “Memory Data Access Control” Command, Bits B5, B6, and B7 as described
 * below.
 */
void ILI9341_init() {
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = DC,
                         .GPIO_Mode = GPIO_Mode_Out_PP,
                         .GPIO_Speed = GPIO_Speed_50MHz,
                     });
    GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                         .GPIO_Pin = RESET,
                         .GPIO_Mode = GPIO_Mode_Out_PP,
                         .GPIO_Speed = GPIO_Speed_2MHz,
                     });

    GPIO_WriteBit(GPIOA, RESET, Bit_RESET);
    systick_delay_ms(10);
    GPIO_WriteBit(GPIOA, RESET, Bit_SET);

    systick_delay_ms(50);

    // Sleep Out
    send_command(0x11);
    // wait 5ms
    systick_delay_ms(5);

    // Power control B
    send_command(0xCF);
    send_data(0X00);
    send_data(0XC1);
    send_data(0X30);

    // Power on sequence control
    send_command(0xED);
    send_data(0X64);
    send_data(0X03);
    send_data(0X12);
    send_data(0X81);

    // Driver timing control A
    send_command(0xE8);
    send_data(0X85);
    send_data(0X11);
    send_data(0X78);

    // Interface Control
    send_command(0xF6);
    send_data(0X01);
    send_data(0X30);
    send_data(0X00);

    // Power control A
    send_command(0xCB);
    send_data(0X39);
    send_data(0X2C);
    send_data(0X00);
    send_data(0X34);
    send_data(0X05);

    // Pump ratio control
    send_command(0xF7);
    send_data(0X20);

    // Driver timing control B
    send_command(0xEA);
    send_data(0X00);
    send_data(0X00);

    // Power Control 1
    send_command(0xC0);
    send_data(0X20);

    // Power Control 2
    send_command(0xC1);
    send_data(0X11);

    // VCOM Control 1
    send_command(0xC5);
    send_data(0X31);
    send_data(0X3C);

    // VCOM Control 2
    send_command(0xC7);
    send_data(0XA9);

    // COLMOD: Pixel Format Set
    send_command(0x3A);
    send_data(0X55);

    /**
     * Memory Access Control
     * MX : X-mirror
     * MY : Y-mirror
     * MV : exchange X Y
     * RGB -> BGR Order
     *
     * [MY MX MV ML BGR MH 0 0]
     *
     * 0x08, 0000_1000,
     * Normal, BGR
     *
     * 0xC8, 1100_1000,
     * Mirror, BGR
     *
     */
    send_command(0x36);
    send_data(0x08);  // Normal
    // send_data(0xC8);  // Mirror

    // Frame Rate Control (In Normal Mode/Full Colors)
    // Frame Rate: 79Hz
    send_command(0xB1);
    send_data(0X00);
    send_data(0X18);

    // Display Inversion Control
    // 0: Line inversion, 1: Frame inversion
    send_command(0xB4);
    send_data(0X00);

    // Enable 3G
    send_command(0xF2);
    send_data(0X00);

    // Gamma Set
    send_command(0x26);
    send_data(0X01);

    // Positive Gamma Correction
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

    // Negative Gamma Correction
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

    // Display ON
    send_command(0x29);
}