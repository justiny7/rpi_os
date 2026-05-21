#include "stepper.h"
#include "sys_timer.h"

Pin dir = { 21 };
Pin step = { 20 };

Note chromatic[] = {
    A4, Bb4, B4, C5, Cs5, D5, Eb5,
    E5, F5, Fs5, G5, Gs5
};

void main() {
    Stepper st;
    stepper_init(&st, dir, step);
    
    for (int i = 0; i < 12; i++) {
        stepper_play_note(&st, chromatic[i], 500);
    }

    while (1) {
        stepper_set_direction(&st, 0);
        stepper_step_forward(&st);
        sys_timer_delay_ms(1000);
        stepper_step_backward(&st);
        sys_timer_delay_ms(1000);
    }
}
