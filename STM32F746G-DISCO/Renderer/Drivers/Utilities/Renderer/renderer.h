#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#define LCD_DISP_PIN GPIO_PIN_12
#define LCD_DISP_GPIO_PORT GPIOI
#define LCD_BL_CTRL_PIN GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT GPIOK
#define SDRAM_START_ADDRESS       ((uint32_t)0xC0000000)
#define MAX_WIDTH ((uint16_t)480)
#define MAX_HEIGHT ((uint16_t)272)

void RENDERER_Init(uint32_t colorFormat);
void RENDERER_MspInit();
void RENDERER_FillRect(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, uint32_t color);
void RENDERER_Copy_FB(uint32_t newFB_Addr, uint32_t oldFB_Addr);
void RENDERER_Swap_FB();
void RENDERER_DMA2D_FillBuffer(void *pDest, uint32_t width, uint32_t height, uint32_t offset, uint32_t color);
#endif /* RENDERER_H */