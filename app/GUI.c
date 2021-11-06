#include "ILI9341.h"
#include "font.h"

// colors
#define WHITE 0xffff
#define GREY 0x4208
#define BLACK 0x0000
#define RED 0xf800
#define BLUE 0x001f
#define GREEN 0x07e0
#define MAGENTA 0xf81f
#define YELLOW 0xffe0
#define CYAN 0x07ff

#define MOD_METHOD

/**
 * @param x 0-319
 * @param y 0-200
 */
static u16 get_grid(u16 x, u16 y) {
#ifdef MOD_METHOD
    if (x != 0 && x != 319 && x != 160) {
        if (y != 0 && y != 100 && y != 200) {
            if (x % 20 != 0 && y % 20 != 0) {
                return BLACK;
            } else {
                return GREY;
            }
        } else {
            return WHITE;
        }
    } else {
        return WHITE;
    }
#endif
}

static void display_grid() {
    ILI9341_set_y(19, 219);
    ILI9341_set_x(0, 319);
    ILI9341_begin_write();
    for (u16 x = 0; x < 320; x++) {
        for (u16 y = 0; y < 201; y++) {
            ILI9341_set_pixel(get_grid(x, y));
        }
    }
}

/**
 * Beware of overflow
 * Assuming size_y is a multiple of 8
 *
 * font:
 *      size_x
 * -----------------
 * | |    /|    /| |
 * | |   / |   / | |
 * | |  /  |  /  | | size_y
 * | | /   | /   | |
 * | v     v     v |
 * -----------------
 */
static void display_char(u16 offset_x,
                         u16 offset_y,
                         u16 size_x,
                         u16 size_y,
                         const u8* font,
                         u16 color) {
    ILI9341_set_y(offset_y, offset_y + size_y - 1);
    ILI9341_set_x(offset_x, offset_x + size_x - 1);
    ILI9341_begin_write();
    for (u8 i = 0; i < (size_y >> 3) * size_x; i++) {
        u8 temp = font[i];
        for (u8 j = 0; j < 8; j++) {
            if (temp & 0x01) {
                ILI9341_set_pixel(color);
            } else {
                ILI9341_set_pixel(0x0000);
            }
            temp >>= 1;
        }
    }
}

/**
 * @param data length = 320, range 0-200
 */
static void display_waveform(const u8* data) {
    static const u8* last_data = 0;
    ILI9341_set_y(0, 0xEF);
    ILI9341_set_x(0, 0x13F);
    if (last_data) {
        // clear last waveform, display persent
        for (u16 t = 0; t < 320; t++) {
            ILI9341_set_y_begin(219 - last_data[t]);
            ILI9341_set_x_begin(t);
            ILI9341_begin_write();
            ILI9341_set_pixel(get_grid(t, 200 - last_data[t]));
            ILI9341_set_y_begin(219 - data[t]);
            ILI9341_begin_write();
            ILI9341_set_pixel(YELLOW);
        }
    } else {
        // last_data = 0, initial waveform
        for (u16 t = 0; t < 320; t++) {
            ILI9341_set_y_begin(219 - data[t]);
            ILI9341_set_x_begin(t);
            ILI9341_begin_write();
            ILI9341_set_pixel(YELLOW);
        }
    }
    last_data = data;
}

void GUI_init() {
    display_grid();
    display_char(0, 224, 8, 16, font_num_8_16[0], YELLOW);
    display_char(8, 224, 8, 16, font_num_8_16[1], YELLOW);
    display_char(16, 224, 8, 16, font_num_8_16[2], YELLOW);
    display_char(24, 224, 8, 16, font_num_8_16[3], YELLOW);
    display_char(32, 224, 8, 16, font_num_8_16[4], YELLOW);
    display_char(40, 224, 8, 16, font_num_8_16[5], YELLOW);
    display_char(48, 224, 8, 16, font_num_8_16[6], YELLOW);
    display_char(56, 224, 8, 16, font_num_8_16[7], YELLOW);
    display_char(64, 224, 8, 16, font_num_8_16[8], YELLOW);
    display_char(72, 224, 8, 16, font_num_8_16[9], YELLOW);
}