#include "usart.h"
#include "main.h"
#include "fifo_usart.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"


static uint8_t FifoInit = 0;


void
Usart_Init(UART_HandleTypeDef *pUsart, uint32_t baud) {
    if(!FifoInit) {// Initialize fifo once
		FifoUsart_Init();
		FifoInit = 1;
	}
    // USART configuration
    pUsart->Instance          = USART2;
    pUsart->Init.BaudRate     = baud;
    pUsart->Init.WordLength   = UART_WORDLENGTH_8B;
    pUsart->Init.StopBits     = UART_STOPBITS_1;
    pUsart->Init.Parity       = UART_PARITY_NONE;
    pUsart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    pUsart->Init.Mode         = UART_MODE_TX_RX;
    pUsart->Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(pUsart) != HAL_OK) {// Calls also HAL_UART_MspInit()
        Error_Handler();
    }
    // Enable USART
    __HAL_UART_ENABLE(pUsart);
    // Enable the Receive interrupt
    __HAL_UART_ENABLE_IT(pUsart, UART_IT_RXNE);
}


void
Usart_Put(UART_HandleTypeDef *usart, bool buffered, unsigned char c) {
    if(buffered) {
        FifoUsart_Insert(USART_DIR_TX, c);
        // Enable sending via interrupt
        Usart_TxInt(usart, true);
    }
    else {
        while(__HAL_UART_GET_FLAG(usart, UART_FLAG_TC) == RESET) {
        }
        HAL_UART_Transmit(usart, &c, 1, 0);
    }
}


void
Usart_Write(UART_HandleTypeDef *usart, bool buffered, unsigned char *data, uint8_t len) {
	uint8_t i = 0;

    if(buffered) {
        while(len--) {
            FifoUsart_Insert(USART_DIR_TX, data[i++]);
        }
        // Enable sending via interrupt
        Usart_TxInt(usart, true);
    }
    else {
        HAL_UART_Transmit(usart, data, len, 100);
        while(__HAL_UART_GET_FLAG(usart, UART_FLAG_TC) == RESET);
    }
}


void
Usart_TxInt(UART_HandleTypeDef *usart, bool enable) {
    if(enable) {
        __HAL_UART_ENABLE_IT(usart, UART_IT_TXE);
	}
    else {
        __HAL_UART_ENABLE_IT(usart, UART_IT_TXE);
	}
}


void
Usart_RxInt(UART_HandleTypeDef *usart, bool enable) {
    if(enable) {
        __HAL_UART_ENABLE_IT(usart, UART_IT_RXNE);
    }
    else {
        __HAL_UART_DISABLE_IT(usart, UART_IT_RXNE);
    }
}

