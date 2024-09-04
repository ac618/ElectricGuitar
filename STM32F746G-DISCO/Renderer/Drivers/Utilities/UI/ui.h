#ifndef __UI_H__
#define __UI_H__

#define SUPPORT_FINGERS 2

#include <stdint.h>
#include "stm32746g_discovery_ts.h"
#include "renderer.h"

typedef enum
{
    TICK = 0x01,
    TOUCHED = 0x02
} ActiveUI_Event;
typedef enum
{
    NONE = 0x01,
    REDRAW = 0x02
} ActiveUI_Request;
typedef enum
{
    UNPRESSED = 0x01,
    PRESSED = 0x02
} Button_State;
typedef struct
{
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
} UI;

typedef struct
{
    UI ui;
    uint32_t defaultColor;
    uint32_t pressedColor;
    Button_State state;
    ActiveUI_Event event;
    ActiveUI_Request request;
    uint8_t justRendered;
} Button;

typedef struct
{
    UI ui;
    uint32_t barColor;
    uint32_t thumbColor;
    uint32_t value;
    ActiveUI_Event event;
    ActiveUI_Request request;
    uint8_t justRendered;
} Slide;

void Button_Init(Button *button, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color);
void Button_Detect(Button *button, uint16_t touchX, uint16_t touchY);
void Button_Update(Button *button);
void Button_Render(Button *button);

void Slide_Init(Slide *slide, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t barColor, uint32_t thumbColor, uint32_t initValue);
void Slide_Detect(Slide *slide, uint16_t touchX, uint16_t touchY);
void Slide_Update(Slide *slide);
void Slide_Render(Slide *slide);

uint32_t DarkenColor(uint32_t color, float factor);

#endif /* __UI_H__ */
