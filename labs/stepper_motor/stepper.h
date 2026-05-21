#ifndef STEPPER_H
#define STEPPER_H

#include "gpio.h"

#define STEPPER_REVOLUTION  200
#define STEPPER_STEP_DELAY  1000

typedef struct {
    int step_count;

    Pin dir;
    Pin step;
} Stepper;

typedef enum {
    A4      = 1130,
    Bb4     = 1050,
    B4      = 1000,
    C5      = 945,
    Cs5     = 890,
    D5      = 840,
    Eb5     = 795,
    E5      = 750,
    F5      = 710,
    Fs5     = 665,
    G5      = 630,
    Gs5      = 590,
} Note;

void stepper_init(Stepper* stepper, Pin dir, Pin step);
void stepper_set_direction(Stepper* stepper, int dir);
void stepper_step(Stepper* stepper, uint32_t steps);

void stepper_play_note(Stepper* stepper, Note note, uint32_t duration);

void stepper_step_forward(Stepper* stepper);
void stepper_step_backward(Stepper* stepper);

#endif

