#include "ssd1306.h"
#include "lib.h"

static bool send_command(SSD1306* oled, uint8_t cmd) {
    uint8_t buf[2] = { SSD1306_I2C_CMD, cmd };
    return i2c_send_data(oled->i2c, 2, buf) == I2C_RESULT_OK;
}
static bool send_command2(SSD1306* oled, uint8_t cmd1, uint8_t cmd2) {
    return send_command(oled, cmd1) &&
        send_command(oled, cmd2);
}
static bool send_command3(SSD1306* oled,
        uint8_t cmd1, uint8_t cmd2, uint8_t cmd3) {
    return send_command(oled, cmd1) &&
        send_command(oled, cmd2) &&
        send_command(oled, cmd3);
}

void ssd1306_init(SSD1306* oled) {
    i2c_init(oled->i2c);
    oled->data_cmd = SSD1306_I2C_DATA;

    ssd1306_display_off(oled);
    ssd1306_set_mux_ratio(oled, 0x3F);
    ssd1306_set_display_start_line(oled);
    ssd1306_set_segment_remap(oled, true);
    ssd1306_set_com_output_scan_dir(oled, true);
    ssd1306_set_com_pins_hw_config(oled, 0x01);
    ssd1306_set_contrast_control(oled, 0x7F); // reset
    ssd1306_display_all_on_resume(oled);
    ssd1306_set_normal_display(oled);
    ssd1306_set_clock_divide_ratio(oled, 0x80);
    ssd1306_set_charge_pump(oled, true);
    ssd1306_set_mem_addr_mode(oled, 0);
    ssd1306_set_col_addr(oled, 0, 127);
    ssd1306_display_on(oled);
}

void ssd1306_display_off(SSD1306* oled) {
    assert(send_command(oled, SSD1306_DISPLAY_OFF), "can't turn off display");
}
void ssd1306_display_on(SSD1306* oled) {
    assert(send_command(oled, SSD1306_DISPLAY_ON), "can't turn on display");
}
void ssd1306_set_mux_ratio(SSD1306* oled, uint32_t mux) {
    assert(mux >= 15 && mux <= 63, "oled invalid mux");
    assert(send_command2(oled, SSD1306_SET_MULTIPLEX_RATIO, mux),
            "can't set mux ratio");
}
void ssd1306_set_display_start_line(SSD1306* oled) {
    assert(send_command(oled, SSD1306_SET_DISPLAY_START_LINE),
            "can't set display start line");
}
void ssd1306_set_segment_remap(SSD1306* oled, bool map_zero) {
    assert(send_command(oled, map_zero ?
            SSD1306_SET_SEGMENT_REMAP_0 :
            SSD1306_SET_SEGMENT_REMAP_127),
            "can't set segment remap");
}
void ssd1306_set_com_output_scan_dir(SSD1306* oled, bool normal) {
    assert(send_command(oled, normal ?
            SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL :
            SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAP),
            "can't set COM output scan direction");
}
void ssd1306_set_com_pins_hw_config(SSD1306* oled, uint32_t cfg) {
    assert((cfg & 0x3) == cfg, "oled invalid COM pin hw config");
    assert(send_command2(oled, SSD1306_SET_COM_PINS_HARDWARE_CONFIG, (cfg << 4) | 2),
            "can't set COM pins hardware config");
}
void ssd1306_set_contrast_control(SSD1306* oled, uint32_t val) {
    assert(send_command2(oled, SSD1306_SET_CONTRAST, val),
            "can't set contrast");
}
void ssd1306_display_all_on_resume(SSD1306* oled) {
    assert(send_command(oled, SSD1306_DISPLAY_ALL_ON_RESUME),
            "can't display all on resume");
}
void ssd1306_set_normal_display(SSD1306* oled) {
    assert(send_command(oled, SSD1306_NORMAL_DISPLAY),
            "can't set normal display");
}
void ssd1306_set_clock_divide_ratio(SSD1306* oled, uint32_t val) {
    assert(send_command2(oled, SSD1306_SET_DISPLAY_CLOCK_DIVIDE_RATIO, val),
            "can't set clock divide");
}
void ssd1306_set_charge_pump(SSD1306* oled, bool on) {
    assert(send_command2(oled, SSD1306_CHARGE_PUMP_SETTING, (((uint32_t) on) << 2) | 0x10),
            "can't set charge pump");
}
void ssd1306_set_mem_addr_mode(SSD1306* oled, uint32_t mode) {
    assert((mode & 0x3) == mode, "oled invalid mem addressing mode");
    assert(send_command2(oled, SSD1306_MEMORY_ADDRESSING_MODE, mode),
            "can't set mem address mode");
}
void ssd1306_set_col_addr(SSD1306* oled, uint32_t lcol, uint32_t rcol) {
    assert(lcol <= rcol && rcol <= 127, "oled invalid columns");
    send_command3(oled, SSD1306_SET_COLUMN_ADDRESS, lcol, rcol);
}
void ssd1306_draw_screen(SSD1306* oled) {
    i2c_send_data(oled->i2c, 1 + SSD1306_BUF_LEN, &oled->data_cmd);
}
void ssd1306_set_pixel(SSD1306* oled, uint32_t y, uint32_t x, bool val) {
    assert(y < SSD1306_HEIGHT, "oled invalid y");
    assert(x < SSD1306_WIDTH, "oled invalid x");

    uint32_t idx = (7 - y / 8) * 128 + (127 - x);
    uint32_t bit = 7 - (y % 8);
    if (val) {
        oled->buf[idx] |= (1 << bit);
    } else {
        oled->buf[idx] &= ~(1 << bit);
    }
}
