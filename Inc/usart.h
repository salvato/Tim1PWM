#pragma once

#include <stdbool.h>
#include "stm32f4xx_hal_uart.h"


// Number of USARTs on this device
#define USART_NUM			3

// Numerate available USARTs in ascending order
#define USART1_NUM			0
#define USART2_NUM			1
#define USART6_NUM			2

// Usart used for Printf(...)
#define STDOUT				USART2
#define STDOUT_NUM			USART2_NUM

// Direction definitions
#define USART_DIR_RX		0
#define USART_DIR_TX		1


#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart2;


void Usart_Init(UART_HandleTypeDef *pUsart, uint32_t baud);

void Usart_Put(USART_TypeDef *usart, bool buffered, char c);
void Usart_Write(USART_TypeDef *usart, bool buffered, char *data, uint8_t len);

void Usart_TxInt(USART_TypeDef *usart, bool enable);
void Usart_RxInt(USART_TypeDef *usart, bool enable);


#ifdef __cplusplus
}
#endif

