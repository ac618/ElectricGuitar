#include "renderer.h"
#include "stm32f7xx_hal.h"

LTDC_HandleTypeDef  hLtdcHandler;
uint8_t activeLayer = 0;
uint32_t renderFB; // Frame buffer to render to the screen
uint32_t drawFB; // Frame buffer to draw onto

void RENDERER_Init(uint32_t colorFormat)
{


}
void RENDERER_FillRect(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height)
{
    uint32_t  x_address;
    
    if (hLtdcHandler.LayerCfg[activeLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    {
        x_address = drawFB + 2*(BSP_LCD_GetXSize()*posY + posX);
    }
    else
    {
        x_address = drawFB + 4*(BSP_LCD_GetXSize()*posY + posX);
    }
    
    /* Fill the rectangle */
    LL_FillBuffer(ActiveLayer, (uint32_t *)x_address, Width, Height, (BSP_LCD_GetXSize() - Width), DrawProp[ActiveLayer].TextColor);

}

void RENDERER_Copy_FB(uint32_t newFB_Addr, uint32_t oldFB_Addr)
{

}

void RENDERER_Swap_FB()
{
    /* swap frame buffer start address */
    uint32_t temp = renderFB;
    renderFB = drawFB;
    drawFB = temp;
    /* reconfigure layer to show swaped frame buffer */
    hLtdcHandler.LayerCfg[activeLayer].FBStartAdress = renderFB;
}