#include "ILI9341.h"
#include "font.h"
#include "status.h"

// colors
#define WHITE (0xffff)
#define GREY (0x4208)
#define BLACK (0x0000)
#define RED (0xf800)
#define BLUE (0x001f)
#define GREEN (0x07e0)
#define MAGENTA (0xf81f)
#define YELLOW (0xffe0)
#define CYAN (0x07ff)

/**
 * Magic code!
 * Linearly map 0-255 to 0-200, and round up
 */
static u8 _255_to_200(u8 num) {
    u16 temp = num * 400 / 255;
    temp += temp & 0x01;
    return temp >> 1;
}

#define MOD_METHOD

/**
 * @param x 0-319
 * @param y 0-200
 * @return color of gird
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

/**
 *   0
 * ...
 *  18
 *  19 --------------------
 * ... |       grid       |
 * 219 --------------------
 * 220
 * ...
 * 239
 */
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
                ILI9341_set_pixel(BLACK);
            }
            temp >>= 1;
        }
    }
}

/**
 * Optimized specifically for n * 8 * 16 chars
 */
static void display_multi_char_8_16(u16 offset_x,
                                    u16 offset_y,
                                    const u8** font,
                                    u8 num,
                                    u16 color) {
    ILI9341_set_y(offset_y, offset_y + 15);
    ILI9341_set_x(offset_x, offset_x + (num << 3) - 1);
    ILI9341_begin_write();
    for (u8 i = 0; i < num; i++) {
        for (u8 j = 0; j < 16; j++) {
            u8 temp = font[i][j];
            for (u8 k = 0; k < 8; k++) {
                if (temp & 0x01) {
                    ILI9341_set_pixel(color);
                } else {
                    ILI9341_set_pixel(BLACK);
                }
                temp >>= 1;
            }
        }
    }
}

/**
 * @brief This function will record the last waveform, and clear
 * the previous one when the next waveform is displayed.
 * @param data length = 320, range 0-255
 */
void GUI_display_waveform(const u8* data) {
    static u8 last_data[320] = {0xff};  // 0-200
    if (status_get_status() == HOLD) {
        return;
    }
    ILI9341_set_y(0, 0xEF);
    ILI9341_set_x(0, 0x13F);
    if (last_data[0] != 0xff) {
        // clear last waveform, display persent
        for (u16 t = 0; t < 320; t++) {
            ILI9341_set_y_begin(219 - last_data[t]);
            ILI9341_set_x_begin(t);
            ILI9341_begin_write();
            ILI9341_set_pixel(get_grid(t, 200 - last_data[t]));
            last_data[t] = _255_to_200(data[t]);
            ILI9341_set_y_begin(219 - last_data[t]);
            ILI9341_begin_write();
            ILI9341_set_pixel(YELLOW);
        }
    } else {
        // initial waveform
        for (u16 t = 0; t < 320; t++) {
            last_data[t] = _255_to_200(data[t]);
            ILI9341_set_y_begin(219 - last_data[t]);
            ILI9341_set_x_begin(t);
            ILI9341_begin_write();
            ILI9341_set_pixel(YELLOW);
        }
    }
}

#define BOTTOM_Y 222

void GUI_display_v_sen(v_sen_t v_sen) {
    const u8* fonts[3];
    if (v_sen <= V0_5) {
        fonts[0] = font_num_8_16[0];
        fonts[1] = font_dot_8_16;
        fonts[2] = font_num_8_16[v_sen];
    } else {
        fonts[0] = font_null_8_16;
        fonts[1] = font_null_8_16;
        fonts[2] = font_num_8_16[v_sen >> 4];
    }
    display_multi_char_8_16(0, BOTTOM_Y, fonts, 3, YELLOW);
}

void GUI_display_coupling(coupling_t coupling) {
    const u8* fonts[3];
    switch (coupling) {
        case DC_coupling:
            fonts[0] = font_D_8_16;
            fonts[1] = font_C_8_16;
            fonts[2] = font_null_8_16;
            break;
        case AC_coupling:
            fonts[0] = font_A_8_16;
            fonts[1] = font_C_8_16;
            fonts[2] = font_null_8_16;
            break;
        case GND_coupling:
            fonts[0] = font_G_8_16;
            fonts[1] = font_N_8_16;
            fonts[2] = font_D_8_16;
            break;
    }
    display_multi_char_8_16(40, BOTTOM_Y, fonts, 3, YELLOW);
}

void GUI_display_time_base(time_base_t time_base) {
    const u8* fonts[4];
    if (time_base & 0x10) {
        // ms
        fonts[3] = font_m_8_16;
    } else {
        // us
        fonts[3] = font_u_8_16;
    }
    if (time_base & 0x20) {
        // one 0
        fonts[2] = font_num_8_16[0];
        fonts[1] = font_num_8_16[time_base & 0x0f];
        fonts[0] = font_null_8_16;
    } else if (time_base & 0x40) {
        // two 0
        fonts[2] = font_num_8_16[0];
        fonts[1] = font_num_8_16[0];
        fonts[0] = font_num_8_16[time_base & 0x0f];
    } else {
        // no 0
        fonts[2] = font_num_8_16[time_base & 0x0f];
        fonts[1] = font_null_8_16;
        fonts[0] = font_null_8_16;
    }
    display_multi_char_8_16(280, BOTTOM_Y, fonts, 4, BLUE);
}

#define TOP_Y 1

void GUI_display_mode(mode_t mode) {
    const u8* fonts[4];
    switch (mode) {
        case ROLL:
            fonts[0] = font_R_8_16;
            fonts[1] = font_O_8_16;
            fonts[2] = font_L_8_16;
            fonts[3] = font_L_8_16;
            break;
        case AUTO:
            fonts[0] = font_A_8_16;
            fonts[1] = font_U_8_16;
            fonts[2] = font_T_8_16;
            fonts[3] = font_O_8_16;
            break;
        case NORMAL:
            fonts[0] = font_N_8_16;
            fonts[1] = font_O_8_16;
            fonts[2] = font_R_8_16;
            fonts[3] = font_M_8_16;
            break;
        case SINGLE:
            fonts[0] = font_S_8_16;
            fonts[1] = font_I_8_16;
            fonts[2] = font_N_8_16;
            fonts[3] = font_G_8_16;
            break;
    }
    display_multi_char_8_16(0, TOP_Y, fonts, 4, CYAN);
}

void GUI_display_trigger(trigger_t trigger) {
    if (trigger == riging_edge) {
        display_char(40, TOP_Y, 16, 16, font_rising_16_16, CYAN);
    } else {
        display_char(40, TOP_Y, 16, 16, font_falling_16_16, CYAN);
    }
}

void GUI_display_status(status_t status) {
    const u8* fonts[4];
    u16 color;
    if (status == RUN) {
        fonts[0] = font_null_8_16;
        fonts[1] = font_R_8_16;
        fonts[2] = font_U_8_16;
        fonts[3] = font_N_8_16;
        color = GREEN;
    } else {
        fonts[0] = font_H_8_16;
        fonts[1] = font_O_8_16;
        fonts[2] = font_L_8_16;
        fonts[3] = font_D_8_16;
        color = RED;
    }
    display_multi_char_8_16(288, TOP_Y, fonts, 4, color);
}

#define BAR_BEGIN 110
#define BAR_TOP_Y 4

/**
 * @param trigger_level 0~255
 */
void GUI_display_trigger_level(u8 trigger_level) {
    static u16 level = 0;
    display_char(BAR_BEGIN - 2 + level, BAR_TOP_Y, 5, 8, font_null_8_16, CYAN);
    level = trigger_level * 200 / 255;
    level += level & 0x01;
    level >>= 1;
    display_char(BAR_BEGIN - 2 + level, BAR_TOP_Y, 5, 8, font_arrow_5_8, CYAN);
}

void GUI_init() {
    // display Black border
    ILI9341_set_y(0, 18);
    ILI9341_set_x(0, 319);
    ILI9341_begin_write();
    for (u16 i = 0; i < 19 * 320; i++) {
        ILI9341_set_pixel(BLACK);
    }
    ILI9341_set_y(220, 239);
    ILI9341_begin_write();
    for (u16 i = 0; i < 20 * 320; i++) {
        ILI9341_set_pixel(BLACK);
    }
    // display fixed UIs
    display_grid();
    display_char(24, BOTTOM_Y, 8, 16, font_V_8_16,
                 YELLOW);  // part of v_sen
    display_char(312, BOTTOM_Y, 8, 16, font_s_8_16,
                 BLUE);  // part of time base
    // bar
    ILI9341_set_y(BAR_TOP_Y + 9, BAR_TOP_Y + 9);
    ILI9341_set_x(BAR_BEGIN, BAR_BEGIN + 100);
    ILI9341_begin_write();
    for (u16 i = 0; i < 101; i++) {
        ILI9341_set_pixel(CYAN);
    }
}