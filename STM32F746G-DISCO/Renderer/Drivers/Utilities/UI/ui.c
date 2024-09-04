#include "ui.h"
#include <stdio.h>

void Button_Init(Button *button, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color)
{
    button->ui.posX = posX;
    button->ui.posY = posY;
    button->ui.width = width;
    button->ui.height = height;
    button->defaultColor = color;
    button->pressedColor = DarkenColor(color, 0.3);
    button->state = UNPRESSED;
    button->event = TICK;
    button->request = NONE;
    button->justRendered = 1;
    RENDERER_FillRect(posX, posY, width, height, button->defaultColor);
}
void Button_Detect(Button *button, uint16_t touchX, uint16_t touchY)
{
    UI ui = button->ui;
    uint8_t xInRange = touchX >= ui.posX && touchX <= ui.posX + ui.width;
    uint8_t yInRange = touchY >= ui.posY && touchY <= ui.posY + ui.height;
    if (xInRange && yInRange)
    {
        button->event = TOUCHED;
    }
    else
    {
        button->event = TICK;
    }
}
void Button_Update(Button *button)
{
    if (button->state == UNPRESSED && button->event == TOUCHED)
    {
        button->state = PRESSED;
        button->request = REDRAW;
        // weak Button_OnClicked();
    }
    if (button->state == PRESSED && button->event == TICK)
    {
        button->state = UNPRESSED;
        button->request = REDRAW;
        // weak Button_OnReleased();
    }
    if (button->event == TICK && button->justRendered == 1)
    {
        button->request = REDRAW;
        button->justRendered == 0;
    }
}
void Button_Render(Button *button)
{
    if (button->state == PRESSED)
    {
        RENDERER_FillRect(button->ui.posX, button->ui.posY, button->ui.width, button->ui.height, button->pressedColor);
    }
    else
    {
        RENDERER_FillRect(button->ui.posX, button->ui.posY, button->ui.width, button->ui.height, button->defaultColor);
    }
    button->request = NONE;
}

void Slide_Init(Slide *slide, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t barColor, uint32_t thumbColor,uint32_t initValue)
{
    /* Init Object */
    slide->ui.posX = posX;
    slide->ui.posY = posY;
    slide->ui.width = width;
    slide->ui.height = height;
    slide->barColor = barColor;
    slide->thumbColor = thumbColor;
    slide->value = initValue;
    slide->event = TICK;
    slide->request = NONE;
    slide->justRendered = 1;
    /* Draw Default Object */
    // Bar
    RENDERER_FillRect(posX, posY + 0.25 * height, width, 0.5 * height, slide->barColor);
    // Thumb
    RENDERER_FillRect(posX + 0.01 * initValue * (width - height), posY, height, height, slide->thumbColor);
}
void Slide_Detect(Slide *slide, uint16_t touchX, uint16_t touchY)
{
    UI ui = slide->ui;
    uint8_t xInRange = touchX >= ui.posX && touchX <= ui.posX + ui.width;
    uint8_t yInRange = touchY >= ui.posY && touchY <= ui.posY + ui.height;
    if (xInRange && yInRange)
    {
        slide->event = TOUCHED;
        slide->value = (touchX - slide->ui.posX - 0.5 * slide->ui.height) / (slide->ui.width - slide->ui.height) * 100;
        if (slide->value > 100)
        {
            slide->value = 100;
        }
    }
    else
    {
        slide->event = TICK;
    }
}
void Slide_Update(Slide *slide)
{
    if (slide->event == TOUCHED)
    {
        slide->request = REDRAW;
        // weak Slide_OnValueChanged();
    }
    if (slide->event == TICK && slide->justRendered == 1)
    {
        slide->request = REDRAW;
        slide->justRendered == 0;
    }
    
}
void Slide_Render(Slide *slide)
{
    uint16_t shiftX = 0.01 * slide->value * (slide->ui.width - slide->ui.height);
    
    // Background
    RENDERER_FillRect(slide->ui.posX, slide->ui.posY, slide->ui.width, slide->ui.height, 0);
    // Bar
    RENDERER_FillRect(slide->ui.posX, slide->ui.posY + 0.25 * slide->ui.height, slide->ui.width, 0.5 * slide->ui.height, slide->barColor);
    // Thumb
    RENDERER_FillRect(slide->ui.posX + shiftX, slide->ui.posY, slide->ui.height, slide->ui.height, slide->thumbColor);

    slide->request = NONE;
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