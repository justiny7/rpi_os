#include "stepper.h"
#include "sys_timer.h"
#include "arm_timer.h"

void stepper_init(Stepper* stepper, Pin dir, Pin step) {
    stepper->dir = dir;
    stepper->step = step;

    gpio_select_output(dir);
    gpio_select_output(step);
}

void stepper_step(Stepper* stepper, uint32_t steps) {
    stepper->step_count += steps;
    while (steps--) {
        gpio_set_high(stepper->step);
        sys_timer_delay_us(1);
        gpio_set_low(stepper->step);
        sys_timer_delay_us(STEPPER_STEP_DELAY);
    }
}
void stepper_set_direction(Stepper* stepper, int dir) {
    if (dir) {
        gpio_set_high(stepper->dir);
    } else {
        gpio_set_low(stepper->dir);
    }
}
void stepper_step_forward(Stepper* stepper) {
    stepper_set_direction(stepper, 0);
    stepper_step(stepper, STEPPER_REVOLUTION);
}
void stepper_step_backward(Stepper* stepper) {
    stepper_set_direction(stepper, 1);
    stepper_step(stepper, STEPPER_REVOLUTION);
}

static void step_delay(Stepper* stepper, uint32_t delay) {
    gpio_set_high(stepper->step);
    sys_timer_delay_us(1);
    gpio_set_low(stepper->step);
    sys_timer_delay_us(delay);
}
void stepper_play_note(Stepper* stepper, Note note, uint32_t duration) {
    uint32_t t = sys_timer_get_usec();
    while (1) {
        stepper_set_direction(stepper, 0);
        step_delay(stepper, note);
        stepper_set_direction(stepper, 1);
        step_delay(stepper, note);

        if (sys_timer_get_usec() - t > duration * 1000) {
            break;
        }
    }
}
