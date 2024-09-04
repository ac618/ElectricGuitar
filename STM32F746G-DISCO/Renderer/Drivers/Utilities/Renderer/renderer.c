#include "renderer.h"
#include "stm32f7xx_hal.h"
#include "rk043fn48h.h"
#include "stm32746g_discovery_sdram.h"
#include <stdio.h>

LTDC_HandleTypeDef  hLtdcHandler;
LTDC_LayerCfgTypeDef layer_cfg;
DMA2D_HandleTypeDef hDma2dHandler;
// uint8_t activeLayer = 0;
uint32_t renderFB; // Frame buffer to render to the screen
uint32_t drawFB; // Frame buffer to draw onto

void RENDERER_Init(uint32_t colorFormat)
{
    /* LTDC Init*/
    // Timing Configuration
    hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
    hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
    hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
    hLtdcHandler.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);
    // Clock Config
    static RCC_PeriphCLKInitTypeDef periph_clk_init_struct;
    periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
    periph_clk_init_struct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
    periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
    // Background value
    hLtdcHandler.Init.Backcolor.Blue = 0;
    hLtdcHandler.Init.Backcolor.Green = 0;
    hLtdcHandler.Init.Backcolor.Red = 0;
    // Polarity
    hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hLtdcHandler.Instance = LTDC;
    if (HAL_LTDC_GetState(&hLtdcHandler) == HAL_LTDC_STATE_RESET)
    {
        /* Initialize the LCD Msp: this __weak function can be rewritten by the application */
        RENDERER_MspInit(&hLtdcHandler, NULL);
    }
    HAL_LTDC_Init(&hLtdcHandler);
    // Turn on display
    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    /* SDRAM Init */
    BSP_SDRAM_Init();

    /* Frame Buffer Init */
    renderFB = SDRAM_START_ADDRESS;
    if (colorFormat == LTDC_PIXEL_FORMAT_RGB565)
    {
        drawFB = renderFB + MAX_WIDTH * MAX_HEIGHT * 2;
    }
    else // ARGB888
    {
        drawFB = renderFB + MAX_WIDTH * MAX_HEIGHT * 4;
    }
    
    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = RK043FN48H_WIDTH;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = RK043FN48H_HEIGHT;
    layer_cfg.PixelFormat = colorFormat;
    layer_cfg.FBStartAdress = renderFB;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = RK043FN48H_WIDTH;
    layer_cfg.ImageHeight = RK043FN48H_HEIGHT;
    HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);

    /* DMA2D Settings */
    hDma2dHandler.Init.Mode = DMA2D_R2M;
    hDma2dHandler.Init.ColorMode = colorFormat;
    hDma2dHandler.Instance = DMA2D;

    /* Fill Buffers With Black  */
    RENDERER_DMA2D_FillBuffer((uint32_t *)renderFB, MAX_WIDTH, MAX_HEIGHT, 0, 0);
    RENDERER_DMA2D_FillBuffer((uint32_t *)drawFB, MAX_WIDTH, MAX_HEIGHT, 0, 0);
}
void RENDERER_MspInit()
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the LTDC and DMA2D clocks */
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();

    /*** LTDC Pins configuration ***/
    /* GPIOE configuration */
    gpio_init_structure.Pin = GPIO_PIN_4;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* GPIOG configuration */
    gpio_init_structure.Pin = GPIO_PIN_12;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* GPIOI LTDC alternate configuration */
    gpio_init_structure.Pin = GPIO_PIN_9 | GPIO_PIN_10 |
                              GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &gpio_init_structure);

    /* GPIOJ configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

    /* GPIOK configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                              GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &gpio_init_structure);

    /* LCD_DISP GPIO configuration */
    gpio_init_structure.Pin = LCD_DISP_PIN; /* LCD_DISP pin has to be manually controlled */
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

    /* LCD_BL_CTRL GPIO configuration */
    gpio_init_structure.Pin = LCD_BL_CTRL_PIN; /* LCD_BL_CTRL pin has to be manually controlled */
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
}
void RENDERER_FillRect(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color)
{
    uint32_t x_address;

    if (hLtdcHandler.LayerCfg[0].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    {
        x_address = drawFB + 2 * (MAX_WIDTH * posY + posX);
    }
    else
    {
        x_address = drawFB + 4 * (MAX_WIDTH * posY + posX);
    }
    
    /* Fill the rectangle */
    RENDERER_DMA2D_FillBuffer((uint32_t *)x_address, width, height, MAX_WIDTH - width, color);
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
    layer_cfg.FBStartAdress = renderFB;
    HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);
}

void RENDERER_DMA2D_FillBuffer(void *pDest, uint32_t width, uint32_t height, uint32_t offset, uint32_t color)
{
    hDma2dHandler.Init.OutputOffset = offset;

    /* DMA2D Initialization */
    if (HAL_DMA2D_Init(&hDma2dHandler) == HAL_OK)
    {
        if (HAL_DMA2D_ConfigLayer(&hDma2dHandler, 0) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hDma2dHandler, color, (uint32_t)pDest, width, height) == HAL_OK)
            {
                /* Polling For DMA transfer */
                HAL_DMA2D_PollForTransfer(&hDma2dHandler, 10);
            }
        }
    }
}