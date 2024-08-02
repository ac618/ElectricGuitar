#include "plotlib.h"
#include "stm32746g_discovery_lcd.h"
#include <stdio.h>
#include <math.h>

void PLT_Plot(float *x, float *y, size_t blockSize, uint16_t origin[2], uint16_t size[2])
{
    /*Get input range*/
    float xMin = x[0];
    float xMax = x[0];
    float yMin = y[0];
    float yMax = y[0];
    for (size_t i = 0; i < blockSize; i++)
    {
        xMin = (x[i] < xMin) ? x[i] : xMin;
        xMax = (x[i] > xMax) ? x[i] : xMax;
        yMin = (y[i] < yMin) ? y[i] : yMin;
        yMax = (y[i] > yMax) ? y[i] : yMax;
    }
    printf("Range: x(%.3f, %.3f), y(%.3f, %.3f)\n", xMin, xMax, yMin, yMax);
    /*Plot Coordinates*/
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DrawHLine(origin[0], origin[1], size[0]);
    BSP_LCD_DrawVLine(origin[0], origin[1] - size[1], size[1]);
    /*Calculate labels*/
    float coef;
    int16_t xOrder, yOrder;
    int16_t xTickMin, xTickMax, yTickMin, yTickMax;
    int16_t xTickInterval, yTickInterval;
    calculateTicks(xMin, xMax, 5, &xTickMin, &xTickMax, &xTickInterval, &xOrder);
    calculateTicks(yMin, yMax, 5, &yTickMin, &yTickMax, &yTickInterval, &yOrder);
    printf("***x*** order: %d, range(%d, %d), interval: %d\n", xOrder, xTickMin, xTickMax, xTickInterval);
    printf("***y*** order: %d, range(%d, %d), interval: %d\n", yOrder, yTickMin, yTickMax, yTickInterval);
    /*Determine unit length*/
    float xUnit = size[0] / (xTickMax - xTickMin);
    float yUnit = size[1] / (yTickMax - yTickMin);
    printf("xUnit: %.3f, yUnit: %.3f\n", xUnit, yUnit);
    /*Plot labels*/
    int16_t currentTick = xTickMin;
    uint16_t tickPos;
    const uint16_t labelHalfLength = 8;
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
    BSP_LCD_SetFont(&Font16);
    char label[5];
    while (currentTick <= xTickMax)
    {
        tickPos = origin[0] + (currentTick - xTickMin) * xUnit;
        BSP_LCD_DrawVLine(tickPos, origin[1] - labelHalfLength, 2 * labelHalfLength);
        if (xOrder <= 2 && xOrder >= -2)
        {
            if (xOrder == -2)
            {
                sprintf(label, "%.2f", (float)currentTick * powf(10, xOrder));
            }
            else if (xOrder == -1)
            {
                sprintf(label, "%.1f", (float)currentTick * powf(10, xOrder));
            }
            else
            {
                sprintf(label, "%d", (int16_t)(currentTick * powf(10, xOrder)));
            }
        }
        else
        {
            sprintf(label, "%d", currentTick);
        }
        BSP_LCD_DisplayStringAt(tickPos - 0.5 * BSP_LCD_GetFont()->Width, origin[1] + labelHalfLength + 2, (uint8_t *)label, LEFT_MODE);
        currentTick += xTickInterval;
    }
    currentTick = yTickMin;
    while (currentTick <= yTickMax)
    {
        tickPos = origin[1] - (currentTick - yTickMin) * yUnit;
        BSP_LCD_DrawHLine(origin[0] - labelHalfLength, tickPos, 2 * labelHalfLength);
        if (yOrder <= 2 && yOrder >= -2)
        {
            if (yOrder == -2)
            {
                sprintf(label, "%.2f", (float)currentTick * powf(10, yOrder));
            }
            else if (yOrder == -1)
            {
                sprintf(label, "%.1f", (float)currentTick * powf(10, yOrder));
            }
            else
            {
                sprintf(label, "%d", (int16_t)(currentTick * powf(10, yOrder)));
            }
        }
        else
        {
            sprintf(label, "%d", currentTick);
        }
        BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize() - origin[0], tickPos - 0.5 * BSP_LCD_GetFont()->Height, (uint8_t *)label, RIGHT_MODE);
        currentTick += yTickInterval;
    }
    BSP_LCD_SetFont(&Font16);
    uint16_t font16Width = BSP_LCD_GetFont()->Width;
    uint16_t font16Height = BSP_LCD_GetFont()->Height;
    if (xOrder < -2 || xOrder > 2)
    {
        BSP_LCD_DisplayStringAt(origin[0] + size[0] + BSP_LCD_GetFont()->Width, origin[1], "x10", LEFT_MODE);

        sprintf(label, "%d", xOrder);
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_DisplayStringAt(origin[0] + size[0] + 3.8 * font16Width, origin[1] - 0.75 * font16Height, label, LEFT_MODE);
    }
    BSP_LCD_SetFont(&Font16);
    if (yOrder < -2 || yOrder > 2)
    {
        BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize() - origin[0], origin[1] - size[1] - BSP_LCD_GetFont()->Height - 10, "x10", RIGHT_MODE);
        sprintf(label, "%d", yOrder);
        BSP_LCD_SetFont(&Font12);
        BSP_LCD_DisplayStringAt(origin[0] - 5, origin[1] - size[1] - 10 - 1.6 * font16Height, label, LEFT_MODE);
    }
    
    // /*Plot data*/
    uint16_t xStart;
    uint16_t yStart;
    uint16_t xEnd;
    uint16_t yEnd;
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    for (size_t i = 0; i < blockSize-1; i++)
    {
        // Cartesian to screen space
        xStart = origin[0] + (x[i] / pow(10, xOrder) - xTickMin) * xUnit;
        yStart = origin[1] - (y[i] / pow(10, yOrder) - yTickMin) * yUnit;
        xEnd = origin[0] + (x[i + 1] / pow(10, xOrder) - xTickMin) * xUnit;
        yEnd = origin[1] - (y[i + 1] / pow(10, yOrder) - yTickMin) * yUnit;

        BSP_LCD_DrawLine(xStart, yStart, xEnd, yEnd);
    }    
}

void calculateTicks(float min, float max, uint16_t labelNum, int16_t *labelMin, int16_t *labelMax, int16_t *labelInterval, int16_t *order)
{
    /*Calculate label interval*/
    float range = max - min;
    float interval = range / (labelNum - 1);

    /*Adjust interval to (1, 2, 5, 10) * 10^n*/
    *order = floorf(log10(interval));
    float normalizedInterval = interval / powf(10, *order);

    if (normalizedInterval < 1.5)
        *labelInterval = 1;
    else if (normalizedInterval < 3)
        *labelInterval = 2;
    else if (normalizedInterval < 7)
        *labelInterval = 5;
    else
        *labelInterval = 10;

    /*Find min max label*/
    *labelMin = floorf((min / powf(10, *order)) / (*labelInterval)) * (*labelInterval);
    *labelMax = ceilf((max / powf(10, *order)) / (*labelInterval)) * (*labelInterval);
}