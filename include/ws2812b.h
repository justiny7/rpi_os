#ifndef WS2812B_H
#define WS2812B_H

#include "gpio.h"
#include <stdint.h>

typedef struct {
    Pin p;
} WS2812B;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void ws2812b_setup(WS2812B* w);
void ws2812b_send_byte(WS2812B* w);
void ws2812b_send_rgb(WS2812B* w, Color c);
void ws2812b_reset(WS2812B* w);

#endif

