#include "engine.h"
#include <stdio.h>

uint8_t update_flag = 0;
uint8_t vsync_flag = 0;
Button button1;
Slide slide1;

void Engine_Init()
{
    Button_Init(&button1, 50, 50, 200, 100, 0x00d2ff);
    Slide_Init(&slide1, 50, 180, 300, 50, 0x9ccea5, 0xffffff, 50);
}

void Engine_Collect()
{
    TS_StateTypeDef tsState;
    BSP_TS_GetState(&tsState);
    uint16_t touchX = 1000;
    uint16_t touchY = 1000;
    if (tsState.touchDetected)
    {
        touchX = tsState.touchX[0];
        touchY = tsState.touchY[0];
    }
    Button_Detect(&button1, touchX, touchY);
    Slide_Detect(&slide1, touchX, touchY);
}
void Engine_Update()
{
    Button_Update(&button1);
    Slide_Update(&slide1);
}
void Engine_Render()
{
    if (button1.request == REDRAW)
    {
        Button_Render(&button1);
        update_flag = 1;
    }
    if (slide1.request == REDRAW)
    {
        Slide_Render(&slide1);
        update_flag = 1;
    }
    if (update_flag == 1)
    {
        RENDERER_Swap_FB();
        update_flag = 0;
    }
}
void Engine_Wait()
{
    vsync_flag = 0;
    while (vsync_flag == 0)
    {
    }
    vsync_flag = 0;
}
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
    __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_LI);
    vsync_flag = 1;
}