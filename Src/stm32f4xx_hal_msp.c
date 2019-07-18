#include "tim.h"
#include "main.h"


/**
  * Initializes the Global MSP.
  */
void
HAL_MspInit(void) {// Called by HAL_Init()
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}


/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void
HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {// Called by HAL_TIM_Base_Init()
    if(htim_base->Instance==TIM1)     {
        // Peripheral clock enable
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    if(htim_base->Instance==TIM9) {
        // Peripheral clock enable
        __HAL_RCC_TIM9_CLK_ENABLE();
        // TIM9 interrupt Init
        HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
    }
}


void
HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(htim->Instance==TIM1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = SpindlePWM_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(SpindlePWM_GPIO_Port, &GPIO_InitStruct);
    }
}


/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void
HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {
    if(htim_base->Instance==TIM1) {
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    if(htim_base->Instance==TIM9) {
        __HAL_RCC_TIM9_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
    }
}


/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void
HAL_UART_MspInit(UART_HandleTypeDef* huart) {// Called by HAL_UART_Init()
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance==USART2) {
// Peripheral clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
// USART2 GPIO Configuration
//   PA2     ------> USART2_TX
//   PA3     ------> USART2_RX
        GPIO_InitStruct.Pin       = USART_TX_Pin;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(USART_TX_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin       = USART_RX_Pin;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(USART_RX_GPIO_Port, &GPIO_InitStruct);

// USART2 interrupt Init
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    else {
        Error_Handler();
    }
}


/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void
HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
    if(huart->Instance==USART2)     {
// Peripheral clock disable
        __HAL_RCC_USART2_CLK_DISABLE();
// USART2 GPIO Configuration
//  PA2     ------> USART2_TX
//  PA3     ------> USART2_RX
        HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);
    }
// USART2 interrupt DeInit
    HAL_NVIC_DisableIRQ(USART2_IRQn);
}
