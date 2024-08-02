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
    /*Calculate tick size*/
    // Ticks should be float not int in case tick < 1
    float xTick = size[0] / (xMax - xMin);
    float yTick = size[1] / (yMax - yMin);
    printf("xTick: %.3f, yTick: %.3f\n", xTick, yTick);
    /*Plot Coordinate*/
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DrawHLine(origin[0], origin[1], size[0]);
    BSP_LCD_DrawVLine(origin[0], origin[1] - size[1], size[1]);
    /*Plot data*/
    uint16_t xStart;
    uint16_t yStart;
    uint16_t xEnd;
    uint16_t yEnd;
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    for (size_t i = 0; i < blockSize-1; i++)
    {
        // Cartesian to screen space
        xStart = origin[0] + (x[i] - xMin) * xTick;
        yStart = origin[1] - (y[i] - yMin) * yTick;
        xEnd = origin[0] + (x[i + 1] - xMin) * xTick;
        yEnd = origin[1] - (y[i + 1] - yMin) * yTick;

        BSP_LCD_DrawLine(xStart, yStart, xEnd, yEnd);
    }
    /*Calculate labels*/
    float coef;
    int16_t xOrder;
    int16_t yOrder;
    scientificNotation(xMin, &coef, &xOrder);
    int16_t xLabelMin = (int16_t)ceil(coef);
    int16_t xLabelMax = floor(xMax / powf(10, xOrder));
    scientificNotation(yMin, &coef, &yOrder);
    int16_t yLabelMin = (int16_t)ceil(coef);
    int16_t yLabelMax = floor(yMax / powf(10, xOrder));
    printf("***x*** order: %d, range(%d, %d)\n", xOrder, xLabelMin, xLabelMax);
    printf("***y*** order: %d, range(%d, %d)\n", yOrder, yLabelMin, yLabelMax);
    /*Show labels*/
    int16_t currentLabel = xLabelMin;
    uint16_t labelPos;
    const uint16_t labelHalfLength = 8;
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_TRANSPARENT);
    BSP_LCD_SetFont(&Font16);
    char label[3];
    while (currentLabel <= xLabelMax)
    {
        labelPos = origin[0] + (currentLabel * powf(10, xOrder) - xMin) * xTick;
        BSP_LCD_DrawVLine(labelPos, origin[1] - labelHalfLength, 2 * labelHalfLength);
        sprintf(label, "%d", currentLabel);
        BSP_LCD_DisplayStringAt(labelPos - 0.5 * BSP_LCD_GetFont()->Width, origin[1] + labelHalfLength + 2, (uint8_t *)label, LEFT_MODE);
        currentLabel++;
    }
    currentLabel = yLabelMin;
    while (currentLabel <= yLabelMax)
    {
        labelPos = origin[1] - (currentLabel * powf(10, yOrder) - yMin) * yTick;
        BSP_LCD_DrawHLine(origin[0] - labelHalfLength, labelPos, 2 * labelHalfLength);
        sprintf(label, "%d", currentLabel);
        BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize() - origin[0], labelPos - 0.5 * BSP_LCD_GetFont()->Height, (uint8_t *)label, RIGHT_MODE);
        currentLabel++;
    }
}

void scientificNotation(float num, float *coef, int16_t *exp)
{
    if (num == 0)
    {
        *coef = 0;
        *exp = 0;
        return;
    }

    // Determine the exponent
    int e = 0;
    float abs_num = fabsf(num);

    while (abs_num < 1.0f)
    {
        abs_num *= 10.0f;
        e--;
    }

    while (abs_num >= 10.0f)
    {
        abs_num /= 10.0f;
        e++;
    }

    // Set the coefficient and exponent
    *coef = num < 0 ? -abs_num : abs_num;
    *exp = (uint16_t)e;
}