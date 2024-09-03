#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#define SDRAM_START_ADDRESS       ((uint32_t)0xC0000000)

void RENDERER_Init(uint32_t colorFormat);
void RENDERER_FillRect(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height);
void RENDERER_Copy_FB(uint32_t newFB_Addr, uint32_t oldFB_Addr);
void RENDERER_Swap_FB();
#endif /* RENDERER_H */