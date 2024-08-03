#include "plotlib.h"
#include "stm32746g_discovery_lcd.h"
#include <stdio.h>
#include <math.h>

#define LABEL_HALF_LENGTH 8

/* Main Plotting Functions */

/**
 * @brief Plots the data on the figure.
 *
 * Initializes the figure's origin and size, and performs the following:
 * - Plots coordinates (axes).
 * - Calculates the range of data.
 * - Determines tick marks and their labels.
 * - Calculates unit lengths for the plot.
 * - Updates the plot with the given data.
 *
 * @param fig Pointer to the Figure structure containing plot configuration.
 * @param x Array of x-coordinates of the data points.
 * @param y Array of y-coordinates of the data points.
 * @param blockSize Number of data points.
 * @param origin Array representing the origin point of the plot (pixel coordinates).
 * @param size Array representing the size of the plot (width and height in pixels).
 */
void PLT_Plot(Figure *fig, float *x, float *y, size_t blockSize, uint16_t origin[2], uint16_t size[2])
{
    /*initialize figure with origin and size*/
    fig->origin[0] = origin[0];
    fig->origin[1] = origin[1];
    fig->size[0] = size[0];
    fig->size[1] = size[1];
    /*Plot Coordinates*/
    plotCoordinates(fig);
    /*Get data range*/
    float xMin, xMax, yMin, yMax;
    findRangef(x, blockSize, &xMin, &xMax);
    findRangef(y, blockSize, &yMin, &yMax);
    /*Determine ticks*/
    fig->xTicks = findTicks(xMin, xMax, 5);
    fig->yTicks = findTicks(yMin, yMax, 5);
    /*Determine unit length (length per pixel)*/
    fig->xUnit = fig->size[0] / (fig->xTicks.max - fig->xTicks.min);
    fig->yUnit = fig->size[1] / (fig->yTicks.max - fig->yTicks.min);
    /*Plot ticks and labels*/
    plotLabels(fig);
    /*Plot data*/
    PLT_UpdatePlot(fig, x, y, blockSize);
}

/**
 * @brief Updates the plot with new data.
 *
 * Draws lines connecting data points on the figure.
 *
 * @param fig Pointer to the Figure structure containing plot configuration.
 * @param x Array of x-coordinates of the data points.
 * @param y Array of y-coordinates of the data points.
 * @param blockSize Number of data points.
 */
void PLT_UpdatePlot(Figure *fig, float *x, float *y, size_t blockSize)
{
    /*Plotting configuration*/
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    /*Plot data*/
    uint16_t xStart;
    uint16_t yStart;
    uint16_t xEnd;
    uint16_t yEnd;
    float xPow = powf(10, fig->xTicks.order);
    float yPow = powf(10, fig->yTicks.order);
    for (size_t i = 0; i < blockSize - 1; i++)
    {
        // Cartesian to screen space
        xStart = fig->origin[0] + (x[i] / xPow - fig->xTicks.min) * fig->xUnit;
        yStart = fig->origin[1] - (y[i] / yPow - fig->yTicks.min) * fig->yUnit;
        xEnd = fig->origin[0] + (x[i + 1] / xPow - fig->xTicks.min) * fig->xUnit;
        yEnd = fig->origin[1] - (y[i + 1] / yPow - fig->yTicks.min) * fig->yUnit;

        BSP_LCD_DrawLine(xStart, yStart, xEnd, yEnd);
    }
}

/* Sub-Plotting Functions */

/**
 * @brief Draws the plot coordinates (axes).
 *
 * Draws horizontal and vertical lines representing the x and y axes.
 *
 * @param fig Pointer to the Figure structure containing plot configuration.
 */
void plotCoordinates(Figure *fig)
{
    /*Plotting configuration*/
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
    /*Plot coordinates*/
    BSP_LCD_DrawHLine(fig->origin[0], fig->origin[1], fig->size[0]);
    BSP_LCD_DrawVLine(fig->origin[0], fig->origin[1] - fig->size[1], fig->size[1]);
}

/**
 * @brief Draws ticks and labels on the plot.
 *
 * Draws ticks and their labels for both x and y axes, including the order of magnitude.
 *
 * @param fig Pointer to the Figure structure containing plot configuration.
 */
void plotLabels(Figure *fig)
{
    /*Plotting configuration*/
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
    BSP_LCD_SetFont(&Font16);
    /*variables*/
    uint16_t tickPos;
    char label[10];
    /*xLabels*/
    int16_t currentTick = fig->xTicks.min;
    while (currentTick <= fig->xTicks.max)
    {
        // Draw ticks
        tickPos = fig->origin[0] + (currentTick - fig->xTicks.min) * fig->xUnit;
        BSP_LCD_DrawVLine(tickPos, fig->origin[1] - LABEL_HALF_LENGTH, 2 * LABEL_HALF_LENGTH);
        // Label text
        getLabelText(label, currentTick, fig->xTicks.order);
        BSP_LCD_DisplayStringAt(tickPos - 0.5 * BSP_LCD_GetFont()->Width, fig->origin[1] + LABEL_HALF_LENGTH + 2, (uint8_t *)label, LEFT_MODE);
        // Increment
        currentTick += fig->xTicks.interval;
    }
    /*yLabels*/
    currentTick = fig->yTicks.min;
    while (currentTick <= fig->yTicks.max)
    {
        // Draw ticks
        tickPos = fig->origin[1] - (currentTick - fig->yTicks.min) * fig->yUnit;
        BSP_LCD_DrawHLine(fig->origin[0] - LABEL_HALF_LENGTH, tickPos, 2 * LABEL_HALF_LENGTH);
        // Label text
        getLabelText(label, currentTick, fig->yTicks.order);
        BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize() - fig->origin[0], tickPos - 0.5 * BSP_LCD_GetFont()->Height, (uint8_t *)label, RIGHT_MODE);
        // Increment
        currentTick += fig->yTicks.interval;
    }
    /*Label order of magnitude*/
    BSP_LCD_SetFont(&Font16);
    uint16_t font16Width = BSP_LCD_GetFont()->Width;
    uint16_t font16Height = BSP_LCD_GetFont()->Height;
    if (fig->xTicks.order < -2 || fig->xTicks.order > 2)
    {
        BSP_LCD_DisplayStringAt(fig->origin[0] + fig->size[0] + BSP_LCD_GetFont()->Width, fig->origin[1], "x10", LEFT_MODE);

        sprintf(label, "%d", fig->xTicks.order);
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_DisplayStringAt(fig->origin[0] + fig->size[0] + 3.8 * font16Width, fig->origin[1] - 0.75 * font16Height, label, LEFT_MODE);
    }
    BSP_LCD_SetFont(&Font16);
    if (fig->yTicks.order < -2 || fig->yTicks.order > 2)
    {
        BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize() - fig->origin[0], fig->origin[1] - fig->size[1] - BSP_LCD_GetFont()->Height - 10, "x10", RIGHT_MODE);
        sprintf(label, "%d", fig->yTicks.order);
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_DisplayStringAt(fig->origin[0] - 5, fig->origin[1] - fig->size[1] - 10 - 1.6 * font16Height, label, LEFT_MODE);
    }
}

/* Utility Functions */

/**
 * @brief Finds the minimum and maximum values in an array of floats.
 *
 * Calculates the minimum and maximum values in the given array.
 *
 * @param num Pointer to the array of float values.
 * @param size Number of elements in the array.
 * @param min Pointer to store the minimum value.
 * @param max Pointer to store the maximum value.
 */
void findRangef(float *num, size_t size, float *min, float *max)
{
    /*Check if the size is valid*/
    if (size <= 0)
    {
        printf("Error: invalid array size");
        return;
    }
    /*Find range*/
    *min = num[0];
    *max = num[0];
    for (size_t i = 1; i < size; i++)
    {
        *min = (num[i] < *min) ? num[i] : *min;
        *max = (num[i] > *max) ? num[i] : *max;
    }
}

/**
 * @brief Calculates tick marks for a given range.
 *
 * Determines the tick marks, their interval, and order of magnitude based on the range and number of ticks.
 *
 * @param min Minimum value of the range.
 * @param max Maximum value of the range.
 * @param num Desired number of ticks.
 *
 * @return A Ticks structure containing the tick marks configuration.
 */
Ticks findTicks(float min, float max, uint16_t num)
{
    Ticks result;
    /*Calculate label interval*/
    float range = max - min;
    float interval = range / (num - 1);

    /*Adjust interval to (1, 2, 5, 10) * 10^n*/
    result.order = floorf(log10(interval));
    float normalizedInterval = interval / powf(10, result.order);

    if (normalizedInterval < 1.5)
        result.interval = 1;
    else if (normalizedInterval < 3)
        result.interval = 2;
    else if (normalizedInterval < 7)
        result.interval = 5;
    else
        result.interval = 10;

    /*Find min max label*/
    result.min = floorf((min / powf(10, result.order)) / result.interval) * result.interval;
    result.max = ceilf(max / powf(10, result.order) / result.interval) * result.interval;

    return result;
}

/**
 * @brief Gets the label text for a tick mark.
 *
 * Formats the tick mark value and its order of magnitude into a string.
 * Order of magnitude within [-2, 2] will  be represented as the original number rather than scientific notation.
 *
 * @param label Pointer to the buffer where the label text will be stored.
 * @param tick The tick value.
 * @param order The order of magnitude for the tick value.
 */
void getLabelText(char* label, int16_t tick, int16_t order)
{
    if (order <= 2 && order >= -2)
    {
        if (order == -2)
        {
            sprintf(label, "%.2f", (float)tick * powf(10, order));
        }
        else if (order == -1)
        {
            sprintf(label, "%.1f", (float)tick * powf(10, order));
        }
        else
        {
            sprintf(label, "%d", (int16_t)(tick * powf(10, order)));
        }
    }
    else
    {
        sprintf(label, "%d", tick);
    }
}