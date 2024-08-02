#ifndef PLOTLIB_H
#define PLOTLIB_H
#include <stdint.h>
#include <stdio.h>

void PLT_Plot(float *x, float *y, size_t blockSize, uint16_t origin[2], uint16_t size[2]);
void calculateTicks(float min, float max, uint16_t labelNum, int16_t *labelMin, int16_t *labelMax, int16_t *labelInterval, int16_t *order);
#endif /*PLOTLIB_H*/