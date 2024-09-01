#include "ui.h"
#include "stm32746g_discovery_lcd.h"
#include <stdio.h>

void UI_Button_Show(Button *button)
{
    BSP_LCD_SetTextColor(button->color);
    BSP_LCD_FillRect(button->posX, button->posY, button->width, button->height);
}
uint8_t UI_Button_Detect(Button *button, TS_StateTypeDef *tsState)
{
    if (tsState->touchDetected)
    {
        for (size_t i = 0; i < SUPPORT_FINGERS; i++)
        {
            uint8_t xInRange = tsState->touchX[i] >= button->posX && tsState->touchX[i] <= button->posX + button->width;
            uint8_t yInRange = tsState->touchY[i] >= button->posY && tsState->touchY[i] <= button->posY + button->height;
            if (xInRange && yInRange)
            {
                BSP_LCD_SetTextColor(DarkenColor(button->color, 0.5));
                BSP_LCD_FillRect(button->posX, button->posY, button->width, button->height);
                return 1;
            }
        }
    }
    else
    {
        BSP_LCD_SetTextColor(button->color);
        BSP_LCD_FillRect(button->posX, button->posY, button->width, button->height);
        return 0;
    }
}

void UI_Slide_Show(Slide *slide)
{
    uint16_t x, y, w, h;
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(slide->posX - 5, slide->posY - 5, slide->width + 10, slide->height + 10);
    // Bar
    x = slide->posX;
    y = slide->posY + 0.25 * slide->height;
    w = slide->width;
    h = 0.5 * slide->height;
    BSP_LCD_SetTextColor(DarkenColor(slide->color, 0.3));
    BSP_LCD_FillRect(x, y, w, h);
    // Thumb
    x = slide->posX + 0.5 * slide->height + 0.01 * slide->value * (slide->width - slide->height);
    y = slide->posY + 0.5 * slide->height;
    BSP_LCD_SetTextColor(slide->color);
    BSP_LCD_FillCircle(x, y, 0.5 * slide->height);
}
uint32_t UI_Slide_Detect(Slide *slide, TS_StateTypeDef *tsState)
{
    if (!tsState->touchDetected)
    {
        return;
    }
    // Get the simultaneous finger number
    uint16_t fingerNum = tsState->touchDetected;
    if (tsState->touchDetected > SUPPORT_FINGERS)
    {
        fingerNum = SUPPORT_FINGERS;
    }
    // Check each finger
    for (size_t i = 0; i < fingerNum; i++)
    {
        uint8_t xInRange = tsState->touchX[i] >= slide->posX && tsState->touchX[i] <= slide->posX + slide->width;
        uint8_t yInRange = tsState->touchY[i] >= slide->posY && tsState->touchY[i] <= slide->posY + slide->height;
        if (xInRange && yInRange)
        {
            slide->value = (tsState->touchX[i] - slide->posX - 0.5 * slide->height) / (slide->width - slide->height) * 100;
            if (slide->value < 0)
            {
                slide = 0;
            }
            if (slide->value > 100)
            {
                slide->value = 100;
            }
            UI_Slide_Show(slide);
        }
    }
}

uint32_t DarkenColor(uint32_t color, float factor)
{
    // Clamp the factor between 0.0 and 1.0
    if (factor < 0)
    {
        factor = 0;
    }
    if (factor > 1)
    {
        factor = 1;
    }
    // Extract rgba
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    // Darken
    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);

    return (a << 24) | (r << 16) | (g << 8) | b;
}