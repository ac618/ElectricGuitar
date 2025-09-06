#include <stdint.h>
#include <usart.h>

int __io_putchar(char ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}