#include "engine.h"

Button buttons = {};
Slide slides = {};

uint16_t button_num;
uint16_t slide_num;

void Collect(TS_StateTypeDef *tsState)
{
    if (tsState->touchDetected)
    {
        uint16_t posX, posY;
        for (size_t i = 0; i < tsState->touchDetected; i++)
        {
            posX = tsState->touchX[i];
            posY = tsState->touchX[i];
            for (size_t i = 0; i < button_num; i++)
            {
                /* code */
            }
            for (size_t i = 0; i < slide_num; i++)
            {
                /* code */
            }
        }
        Update();
    }
    
}
void Update()
{
    uint8_t updated = 0;
    for (size_t i = 0; i < button_num; i++)
    {
        /* code */
    }
    for (size_t i = 0; i < slide_num; i++)
    {
        /* code */
    }
    if (updated = 1)
    {
        Render();
    }
    
    
}
void Render()
{
    RENDERER_Swap_FB();
}