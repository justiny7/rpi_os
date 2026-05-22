#ifndef SSD1306_H
#define SSD1306_H

#include "i2c.h"

#define SSD1306_DISPLAY_ADDR 0x3C
#define SSD1306_BUF_LEN 1024
#define SSD1306_I2C_CMD 0x00
#define SSD1306_I2C_DATA 0x40

#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128

typedef enum {
    SSD1306_SET_CONTRAST                          = 0x81,
    SSD1306_DISPLAY_ALL_ON_RESUME                 = 0xA4,
    SSD1306_DISPLAY_ALL_ON                        = 0xA5,
    SSD1306_NORMAL_DISPLAY                        = 0xA6,
    SSD1306_INVERT_DISPLAY                        = 0xA7,
    SSD1306_DISPLAY_OFF                           = 0xAE,
    SSD1306_DISPLAY_ON                            = 0xAF,

    SSD1306_RIGHT_HORIZONTAL_SCROLL               = 0x26,
    SSD1306_LEFT_HORIZONTAL_SCROLL                = 0x27,
    SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  = 0x29,
    SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   = 0x2A,
    SSD1306_DEACTIVATE_SCROLL                     = 0x2E,
    SSD1306_ACTIVATE_SCROLL                       = 0x2F,
    SSD1306_SET_VERTICAL_SCROLL_AREA              = 0xA3,

    SSD1306_SET_LOWER_COLUMN_START_ADDRESS        = 0x00,
    SSD1306_SET_HIGHER_COLUMN_START_ADDRESS       = 0x10,
    SSD1306_MEMORY_ADDRESSING_MODE                = 0x20,
    SSD1306_SET_COLUMN_ADDRESS                    = 0x21,
    SSD1306_SET_PAGE_ADDRESS                      = 0x22,
    SSD1306_SET_PAGE_START_ADDRESS                = 0xB0,

    SSD1306_SET_DISPLAY_START_LINE                = 0x40,
    SSD1306_SET_SEGMENT_REMAP_0                   = 0xA0,
    SSD1306_SET_SEGMENT_REMAP_127                 = 0xA1,
    SSD1306_SET_MULTIPLEX_RATIO                   = 0xA8,
    SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL  = 0xC0,
    SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAP   = 0xC8,
    SSD1306_SET_DISPLAY_OFFSET                    = 0xD3,
    SSD1306_SET_COM_PINS_HARDWARE_CONFIG          = 0xDA,

    SSD1306_SET_DISPLAY_CLOCK_DIVIDE_RATIO        = 0xD5,
    SSD1306_SET_PRECHARGE_PERIOD                  = 0xD9,
    SSD1306_SET_VCOMH_DESELECT_LEVEL              = 0xDB,
    SSD1306_NOP                                   = 0xE3,

    SSD1306_CHARGE_PUMP_SETTING                   = 0x8D
} SSD1306_Commands;

typedef struct __attribute__((packed)) {
    I2C* i2c;
    uint8_t data_cmd;
    uint8_t buf[1024];
} SSD1306;


void ssd1306_init(SSD1306* oled);

void ssd1306_display_off(SSD1306* oled);
void ssd1306_display_on(SSD1306* oled);
void ssd1306_set_mux_ratio(SSD1306* oled, uint32_t mux);
void ssd1306_set_display_start_line(SSD1306* oled);
void ssd1306_set_segment_remap(SSD1306* oled, bool map_zero);
void ssd1306_set_com_output_scan_dir(SSD1306* oled, bool normal);
void ssd1306_set_com_pins_hw_config(SSD1306* oled, uint32_t cfg);
void ssd1306_set_contrast_control(SSD1306* oled, uint32_t val);
void ssd1306_display_all_on_resume(SSD1306* oled);
void ssd1306_set_normal_display(SSD1306* oled);
void ssd1306_set_clock_divide_ratio(SSD1306* oled, uint32_t val);
void ssd1306_set_charge_pump(SSD1306* oled, bool on);
void ssd1306_set_mem_addr_mode(SSD1306* oled, uint32_t mode);
void ssd1306_set_col_addr(SSD1306* oled, uint32_t lcol, uint32_t rcol);
void ssd1306_set_page_addr(SSD1306* oled, uint32_t pg_st, uint32_t pg_en);

void ssd1306_draw_screen(SSD1306* oled);
void ssd1306_set_pixel(SSD1306* oled, uint32_t y, uint32_t x, bool val);

#endif
