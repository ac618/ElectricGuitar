#include "uart_printf.h"

/* Variables */
UART_HandleTypeDef *huart_print;

/* Fuctions*/
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(huart_print, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

void UART_Printf_Init(UART_HandleTypeDef *huart)
{
    huart_print = huart;
}