#ifndef PLOTLIB_H
#define PLOTLIB_H
#include <stdint.h>
#include <stdio.h>

void PLT_Plot(float *x, float *y, size_t blockSize, uint16_t origin[2], uint16_t size[2]);
void scientificNotation(float num, float *coef, int16_t *exp);
#endif /*PLOTLIB_H*/