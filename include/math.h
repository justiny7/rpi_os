#ifndef MATH_H
#define MATH_H

#include <stdint.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define M_PI    3.14159265358979
#define FINF    __builtin_inff()

float logf(float x);
float sqrtf(float x);
float powf(float b, float e);
float expf(float x);
float sinf(float x);
float cosf(float x);
float tanf(float x);

uint32_t clampd(uint32_t x, uint32_t l, uint32_t r);
float clampf(float x, float l, float r);

#endif
