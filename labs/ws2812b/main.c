#include "ws2812b.h"
#include "sys_timer.h"

#define N 30

Color c[N];

void main() {
    WS2812B w = {
        .p = { 21 }
    };

    ws2812b_setup(&w);

    c[0] = (Color) { 0xFF, 0x00, 0x00 }; // red
    c[1] = (Color) { 0xFF, 0x55, 0x00 };
    c[2] = (Color) { 0xFF, 0xAA, 0x00 };
    c[3] = (Color) { 0xFF, 0xFF, 0x00 }; // yellow
    c[4] = (Color) { 0x55, 0xFF, 0x00 };
    c[5] = (Color) { 0x00, 0xFF, 0x55 };
    c[6] = (Color) { 0x00, 0xFF, 0xFF }; // cyan
    c[7] = (Color) { 0x00, 0x55, 0xFF };
    c[8] = (Color) { 0x00, 0x00, 0xFF }; // blue
    c[9] = (Color) { 0x7F, 0x00, 0xFF }; // violet

    int o = 0;
    while (1) {
        for (int i = 0; i < N; i++) {
            ws2812b_send_rgb(&w, c[(i + o) % N]);
        }
        sys_timer_delay_ms(50);
        o++;
    }
}
