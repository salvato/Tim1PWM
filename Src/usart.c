/*
  USART.c - USART implementation
  Part of STM32F4_HAL

  Copyright (c)	2017 Patrick F.

  STM32F4_HAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  STM32F4_HAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with STM32F4_HAL.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include "usart.h"
#include "main.h"
#include "fifo_usart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "misc.h"


static uint8_t FifoInit = 0;


void
Usart_Init(UART_HandleTypeDef *pUsart, uint32_t baud) {
	GPIO_InitTypeDef GPIO_InitStructure;

	if(!FifoInit) {
		// Initialize fifo once
		FifoUsart_Init();
		FifoInit = 1;
	}

    if(pUsart->Instance == USART2) {
        pUsart->Init.BaudRate     = baud;
        pUsart->Init.WordLength   = UART_WORDLENGTH_8B;
        pUsart->Init.StopBits     = UART_STOPBITS_1;
        pUsart->Init.Parity       = UART_PARITY_NONE;
        pUsart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
        pUsart->Init.Mode         = UART_MODE_TX_RX;
        pUsart->Init.OverSampling = UART_OVERSAMPLING_8;

        /* USART configuration */
        if (HAL_UART_Init(pUsart) != HAL_OK) {
            Error_Handler();
        }
    }
    else {
        Error_Handler();
    }

    // Enable the Receive interrupt
    __HAL_UART_ENABLE_IT(pUsart, UART_IT_RXNE);

    // Enable USART
    __HAL_UART_ENABLE(pUsart);
}


void
Usart_Put(USART_TypeDef *usart, bool buffered, char c) {
    uint8_t num = 0;

    if(usart == USART1) {
        num = USART1_NUM;
    }
    else if(usart == USART2) {
        num = USART2_NUM;
    }
    else if(usart == USART6) {
        num = USART6_NUM;
    }

    if(buffered) {
        FifoUsart_Insert(num, USART_DIR_TX, c);
        // Enable sending via interrupt
        Usart_TxInt(usart, true);
    }
    else {
        while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET) {
        }
		USART_SendData(usart, c);
    }
}


void
Usart_Write(USART_TypeDef *usart, bool buffered, char *data, uint8_t len) {
	uint8_t i = 0;
	uint8_t num = 0;

    if(usart == USART1) {
        num = USART1_NUM;
    }
    else if(usart == USART2) {
        num = USART2_NUM;
    }
    else if(usart == USART6) {
        num = USART6_NUM;
    }

    if(buffered) {
        while(len--) {
            FifoUsart_Insert(num, USART_DIR_TX, data[i++]);
        }

        // Enable sending via interrupt
        Usart_TxInt(usart, true);
    }
    else {
        while(len--) {
            while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET);
            USART_SendData(usart, data[i++]);
        }
    }
}


void
Usart_TxInt(USART_TypeDef *usart, bool enable) {
    if(enable) {
		USART_ITConfig(usart, USART_IT_TXE, ENABLE);
	}
    else {
		USART_ITConfig(usart, USART_IT_TXE, DISABLE);
	}
}


void
Usart_RxInt(USART_TypeDef *usart, bool enable) {
    if(enable) {
		USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
	}
    else {
		USART_ITConfig(usart, USART_IT_RXNE, DISABLE);
	}
}

