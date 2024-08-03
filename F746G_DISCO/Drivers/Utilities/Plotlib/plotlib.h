/* Define to prevent recursive inclusion */
#ifndef PLOTLIB_H
#define PLOTLIB_H

/* Includes */
#include <stdint.h>
#include <stdio.h>

/* Structures */

/**
 * @brief Structure representing ticks shown on a figure.
 */
typedef struct
{
    int16_t min;      /**< Minimum tick value, represented as min * 10^{order}. */
    int16_t max;      /**< Maximum tick value, represented as max * 10^{order}. */
    int16_t interval; /**< Distance between two ticks. */
    int16_t order;    /**< Order of magnitude for the tick values. */
} Ticks;

/**
 * @brief Structure storing configurations of a figure.
 */
typedef struct
{
    uint16_t origin[2]; /**< Origin point of the plot (pixel coordinates). */
    uint16_t size[2];   /**< Size of the plot (width and height in pixels). */
    Ticks xTicks;       /**< Ticks configuration for the x-axis. */
    Ticks yTicks;       /**< Ticks configuration for the y-axis. */
    float xUnit;        /**< Length per pixel for the x-axis. */
    float yUnit;        /**< Length per pixel for the y-axis. */
} Figure;

/* Main plotting functions */

void PLT_Plot(Figure *fig, float *x, float *y, size_t blockSize, uint16_t origin[2], uint16_t size[2]);
void PLT_UpdatePlot(Figure *fig, float *x, float *y, size_t blockSize);

/* Sub-plotting functions */

void plotCoordinates(Figure *fig);
void plotLabels(Figure *fig);

/* Utility functions */

void findRangef(float *num, size_t size, float *min, float *max);
Ticks findTicks(float min, float max, uint16_t num);
void getLabelText(char *label, int16_t tick, int16_t order);

#endif /* PLOTLIB_H */