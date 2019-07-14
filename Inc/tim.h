#pragma once

#include "stm32f4xx_hal.h"

#define SpindlePWM_GPIO_Port GPIOA
#define SpindlePWM_Pin	     GPIO_PIN_7

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim9;
extern TIM_OC_InitTypeDef sConfigOC;

void TIM1_Init(void);
void TIM9_Init(void);
