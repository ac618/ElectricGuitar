#ifndef __UI_H__
#define __UI_H__

#define SUPPORT_FINGERS 2

#include <stdint.h>
#include "stm32746g_discovery_ts.h"
#include "renderer.h"

typedef struct
{
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
    uint32_t color;
} UI;

typedef struct
{
    UI ui;
} Button;

typedef struct
{
    UI ui;
    uint32_t value;
} Slide;

void UI_DrawButton(Button *button);
uint8_t UI_DetectButton(Button *button, TS_StateTypeDef *tsState);
uint8_t UI_UpdateButton(Button *button, TS_StateTypeDef *tsState);

void UI_DrawSlide(Slide *slide);
uint32_t UI_DetectSlide(Slide *slide, TS_StateTypeDef *tsState);
uint8_t UI_UpdateSlide(Button *button, TS_StateTypeDef *tsState);
uint32_t DarkenColor(uint32_t color, float factor);

#endif /* __UI_H__ */
