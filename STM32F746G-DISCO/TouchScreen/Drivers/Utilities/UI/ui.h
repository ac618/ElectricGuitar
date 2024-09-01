#ifndef __UI_H__
#define __UI_H__

#define SUPPORT_FINGERS 2

#include <stdint.h>
#include "stm32746g_discovery_ts.h"

typedef struct
{
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
    uint32_t color;
} Button;

typedef struct
{
    uint16_t posX;
    uint16_t posY;
    uint16_t width;
    uint16_t height;
    uint32_t color;
    uint32_t value;
} Slide;

void UI_Button_Show(Button *button);
uint8_t UI_Button_Detect(Button *button, TS_StateTypeDef *tsState);

void UI_Slide_Show(Slide *slide);
uint32_t UI_Slide_Detect(Slide *slide, TS_StateTypeDef *tsState);
uint32_t DarkenColor(uint32_t color, float factor);

#endif /* __UI_H__ */
