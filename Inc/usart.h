#pragma once

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"


// Usart used for Printf(...)
#define STDOUT				USART2

// Direction definitions
#define USART_DIR_RX		0
#define USART_DIR_TX		1


#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart2;


void Usart_Init(UART_HandleTypeDef *pUsart, uint32_t baud);

void Usart_Put(UART_HandleTypeDef *usart, bool buffered, unsigned char c);
void Usart_Write(UART_HandleTypeDef *usart, unsigned char *data, uint8_t len);

void Usart_TxInt(UART_HandleTypeDef *usart, bool enable);
void Usart_RxInt(UART_HandleTypeDef *usart, bool enable);


#ifdef __cplusplus
}
#endif

