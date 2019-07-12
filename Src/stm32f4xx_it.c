/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "usart.h"
#include "fifo_usart.h"
#include "Limits.h"
#include "Stepper.h"
#include "System.h"
#include "Settings.h"
#include "Config.h"
#include "MotionControl.h"
#include "Platform.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart2;


/* USER CODE BEGIN EV */

void
ProcessReceive(unsigned char c) {
    // Pick off realtime command characters directly from the serial stream. These characters are
    // not passed into the main buffer, but these set system state flag bits for realtime execution.
    switch(c)
    {
    case CMD_RESET:         MC_Reset(); break; // Call motion control reset routine.
    case CMD_RESET_HARD:    NVIC_SystemReset();     // Perform hard reset
    case CMD_STATUS_REPORT: System_SetExecStateFlag(EXEC_STATUS_REPORT);break;
    case CMD_CYCLE_START:   System_SetExecStateFlag(EXEC_CYCLE_START); break; // Set as true
    case CMD_FEED_HOLD:     System_SetExecStateFlag(EXEC_FEED_HOLD); break; // Set as true
    case CMD_STEPPER_DISABLE:     Stepper_Disable(1); break; // Set as true

    default:
        if(c > 0x7F) { // Real-time control characters are extended ACSII only.
            switch(c)
            {
            case CMD_SAFETY_DOOR: System_SetExecStateFlag(EXEC_SAFETY_DOOR); break; // Set as true
            case CMD_JOG_CANCEL:
                if(sys.state & STATE_JOG) { // Block all other states from invoking motion cancel.
                    System_SetExecStateFlag(EXEC_MOTION_CANCEL);
                }
                break;

            case CMD_FEED_OVR_RESET: System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_RESET); break;
            case CMD_FEED_OVR_COARSE_PLUS: System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_COARSE_PLUS); break;
            case CMD_FEED_OVR_COARSE_MINUS: System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_COARSE_MINUS); break;
            case CMD_FEED_OVR_FINE_PLUS: System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_FINE_PLUS); break;
            case CMD_FEED_OVR_FINE_MINUS: System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_FINE_MINUS); break;
            case CMD_RAPID_OVR_RESET: System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_RESET); break;
            case CMD_RAPID_OVR_MEDIUM: System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_MEDIUM); break;
            case CMD_RAPID_OVR_LOW: System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_LOW); break;
            case CMD_SPINDLE_OVR_RESET: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_RESET); break;
            case CMD_SPINDLE_OVR_COARSE_PLUS: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_COARSE_PLUS); break;
            case CMD_SPINDLE_OVR_COARSE_MINUS: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_COARSE_MINUS); break;
            case CMD_SPINDLE_OVR_FINE_PLUS: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_FINE_PLUS); break;
            case CMD_SPINDLE_OVR_FINE_MINUS: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_FINE_MINUS); break;
            case CMD_SPINDLE_OVR_STOP: System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_STOP); break;
            case CMD_COOLANT_FLOOD_OVR_TOGGLE: System_SetExecAccessoryOverrideFlag(EXEC_COOLANT_FLOOD_OVR_TOGGLE); break;
#ifdef ENABLE_M7
            case CMD_COOLANT_MIST_OVR_TOGGLE: System_SetExecAccessoryOverrideFlag(EXEC_COOLANT_MIST_OVR_TOGGLE); break;
#endif
            }
        // Throw away any unfound extended-ASCII character by not passing it to the serial buffer.
        }
        else {
            // Write character to buffer
            FifoUsart_Insert(USART2_NUM, USART_DIR_RX, c);
        }
    }
}

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void
NMI_Handler(void) {
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}


/**
  * @brief This function handles Hard fault interrupt.
  */
void __attribute__ ((noreturn))
HardFault_Handler(void) {
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1) {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}


/**
  * @brief This function handles Memory management fault.
  */
void __attribute__ ((noreturn))
MemManage_Handler(void) {
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1) {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}


/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void __attribute__ ((noreturn))
BusFault_Handler(void) {
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1) {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}


/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void __attribute__ ((noreturn))
UsageFault_Handler(void) {
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1) {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}


/**
  * @brief This function handles System service call via SWI instruction.
  */
void
SVC_Handler(void) {
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}


/**
  * @brief This function handles Debug monitor.
  */
void
DebugMon_Handler(void) {
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}


/**
  * @brief This function handles Pendable request for system service.
  */
void
PendSV_Handler(void) {
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}


/**
  * @brief This function handles System tick timer.
  */
void
SysTick_Handler(void) {
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}


/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/


/**
  * @brief This function handles TIM1 break interrupt and TIM9 global interrupt.
  */
void
TIM1_BRK_TIM9_IRQHandler(void) {
    if (__HAL_TIM_GET_FLAG(&htim9, TIM_FLAG_CC1) != RESET) {
// OC
//        Stepper_MainISR();
        __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_CC1);
    }
    if (__HAL_TIM_GET_FLAG(&htim9, TIM_FLAG_UPDATE) != RESET) {
// OVF
//        Stepper_PortResetISR();
        __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
    }
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}


/**
  * @brief This function handles USART2 global interrupt.
  */
void
USART2_IRQHandler(void) {
    if(__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET) {
        // Read one byte from the receive data register
        unsigned char c;
        HAL_UART_Receive(&huart2, &c, 1, 0);
        ProcessReceive(c);
    }

    if(__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET) {
        unsigned char c;
        if(FifoUsart_Get(USART2_NUM, USART_DIR_TX, &c) == 0) {
            // Write one byte to the transmit data register
            while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET);
            HAL_UART_Transmit(&huart2, &c, 1, 0);
        }
        else {
            // Nothing to transmit - disable interrupt
            __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
        }
    }

    // If overrun condition occurs, clear the ORE flag and recover communication
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET) {
        unsigned char c;
        HAL_UART_Receive(&huart2, &c, 1, 0);
    }
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
