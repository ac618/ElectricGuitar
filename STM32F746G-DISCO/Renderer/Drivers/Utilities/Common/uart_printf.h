#ifndef UART_PRINT_H
#define UART_PRINT_H

/* Includes */
#include "stm32f7xx_hal.h" // change this for different board
#include <stdint.h>

/* Defines */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/* Function Prototypes */
void UART_Printf_Init(UART_HandleTypeDef *huart);

#endif /* UART_PRINTF_H */