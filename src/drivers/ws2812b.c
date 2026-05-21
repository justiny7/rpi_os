#include "ws2812b.h"
#include "gpio.h"
#include "lib.h"
#include "sys_timer.h"

#define GET(x) (*((volatile uint32_t*) x))
#define PUT(x, y) *((volatile uint32_t*) x) = y

static inline void gpio_raw_set_low(uint32_t p_num) {
    const uint32_t register_offset = (p_num >> 5) * REG_SIZE_BYTES;
    const uint32_t val_bit = 1U << (p_num & 31);
    PUT(GPCLR_BASE + register_offset, val_bit);
}
static inline void gpio_raw_set_high(uint32_t p_num) {
    const uint32_t register_offset = (p_num >> 5) * REG_SIZE_BYTES;
    const uint32_t val_bit = 1U << (p_num & 31);
    PUT(GPSET_BASE + register_offset, val_bit);
}

static inline PinOutput gpio_raw_read(uint32_t p_num) {
    const uint32_t offset = (p_num >> 5) * REG_SIZE_BYTES;
    const uint32_t shift = p_num & 31;

    return (PinOutput) ((GET(GPLEV_BASE + offset) >> shift) & 1);
}

void nop() {
    asm volatile ("nop");
}

void ws2812b_setup(WS2812B* w) {
    gpio_select_output(w->p);
    gpio_raw_set_low(w->p.p_num);
    sys_timer_delay_us(5);
}
void send_byte(WS2812B* w, uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        if ((b >> i) & 1) {
            gpio_raw_set_high(w->p.p_num);
            nop(); nop(); nop(); nop();
            gpio_raw_set_low(w->p.p_num);
        } else {
            gpio_raw_set_high(w->p.p_num);
            gpio_raw_set_low(w->p.p_num);
            nop(); nop(); nop(); nop();
        }
    }
}
void ws2812b_send_rgb(WS2812B* w, Color c) {
    send_byte(w, c.g);
    send_byte(w, c.r);
    send_byte(w, c.b);
}
void ws2812b_reset(WS2812B* w) {
    gpio_raw_set_low(w->p.p_num);
    sys_timer_delay_us(5);
}

