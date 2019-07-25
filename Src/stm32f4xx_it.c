#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

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
#include "Report.h"

volatile uint8_t DebounceCounterControl = 0;
volatile uint8_t DebounceCounterLimits = 0;
volatile uint32_t gMillis = 0;// Counter for milliseconds

extern void Limit_PinChangeISR(void);
extern void System_PinChangeISR(void);

extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart2;


uint32_t
millis(void) {
    return gMillis;
}


void
ProcessReceive(unsigned char c) {
    // Pick off realtime command characters directly from the serial stream. These characters are
    // not passed into the main buffer, but these set system state flag bits for realtime execution.
    switch(c)
    {
    case CMD_RESET:
        MC_Reset();
        break; // Call motion control reset routine.
    case CMD_RESET_HARD:
        NVIC_SystemReset();     // Perform hard reset
    case CMD_STATUS_REPORT:
        System_SetExecStateFlag(EXEC_STATUS_REPORT);
        break;
    case CMD_CYCLE_START:
        System_SetExecStateFlag(EXEC_CYCLE_START);
        break; // Set as true
    case CMD_FEED_HOLD:
        System_SetExecStateFlag(EXEC_FEED_HOLD);
        break; // Set as true
    case CMD_STEPPER_DISABLE:
        Stepper_Disable(1);
        break; // Set as true

    default:
        if(c > 0x7F) { // Real-time control characters are extended ASCII only.
            switch(c)
            {
            case CMD_SAFETY_DOOR:
                System_SetExecStateFlag(EXEC_SAFETY_DOOR);
                break; // Set as true
            case CMD_JOG_CANCEL:
                if(sys.state & STATE_JOG) { // Block all other states from invoking motion cancel.
                    System_SetExecStateFlag(EXEC_MOTION_CANCEL);
                }
                break;
            case CMD_FEED_OVR_RESET:
                System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_RESET);
                break;
            case CMD_FEED_OVR_COARSE_PLUS:
                System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_COARSE_PLUS);
                break;
            case CMD_FEED_OVR_COARSE_MINUS:
                System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_COARSE_MINUS);
                break;
            case CMD_FEED_OVR_FINE_PLUS:
                System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_FINE_PLUS);
                break;
            case CMD_FEED_OVR_FINE_MINUS:
                System_SetExecMotionOverrideFlag(EXEC_FEED_OVR_FINE_MINUS);
                break;
            case CMD_RAPID_OVR_RESET:
                System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_RESET);
                break;
            case CMD_RAPID_OVR_MEDIUM:
                System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_MEDIUM);
                break;
            case CMD_RAPID_OVR_LOW:
                System_SetExecMotionOverrideFlag(EXEC_RAPID_OVR_LOW);
                break;
            case CMD_SPINDLE_OVR_RESET:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_RESET);
                break;
            case CMD_SPINDLE_OVR_COARSE_PLUS:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_COARSE_PLUS);
                break;
            case CMD_SPINDLE_OVR_COARSE_MINUS:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_COARSE_MINUS);
                break;
            case CMD_SPINDLE_OVR_FINE_PLUS:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_FINE_PLUS);
                break;
            case CMD_SPINDLE_OVR_FINE_MINUS:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_FINE_MINUS);
                break;
            case CMD_SPINDLE_OVR_STOP:
                System_SetExecAccessoryOverrideFlag(EXEC_SPINDLE_OVR_STOP);
                break;
            case CMD_COOLANT_FLOOD_OVR_TOGGLE:
                System_SetExecAccessoryOverrideFlag(EXEC_COOLANT_FLOOD_OVR_TOGGLE);
                break;
#ifdef ENABLE_M7
            case CMD_COOLANT_MIST_OVR_TOGGLE:
                System_SetExecAccessoryOverrideFlag(EXEC_COOLANT_MIST_OVR_TOGGLE);
                break;
#endif
            }
            // Throw away any unfound extended-ASCII character by not passing it to the serial buffer.
        }
        else {// c < 0x7F
            // Write character to buffer
            FifoUsart_Insert(USART_DIR_RX, c);
        }
    }// default:
}


/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void
NMI_Handler(void) {
}


/**
  * @brief This function handles Hard fault interrupt.
  */
void __attribute__ ((noreturn))
HardFault_Handler(void) {
    while (1) {
    }
}


/**
  * @brief This function handles Memory management fault.
  */
void __attribute__ ((noreturn))
MemManage_Handler(void) {
    while (1) {
    }
}


/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void __attribute__ ((noreturn))
BusFault_Handler(void) {
    while (1) {
    }
}


/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void __attribute__ ((noreturn))
UsageFault_Handler(void) {
    while (1) {
    }
}


/**
  * @brief This function handles System service call via SWI instruction.
  */
void
SVC_Handler(void) {
}


/**
  * @brief This function handles Debug monitor.
  */
void
DebugMon_Handler(void) {
}


/**
  * @brief This function handles Pendable request for system service.
  */
void
PendSV_Handler(void) {
}


/**
  * @brief This function handles System tick timer.
  */
void
SysTick_Handler(void) {
    HAL_IncTick();

    // Because of the board layout, we cant attach all pins to interrupts.
    // Therefore we just poll them in this 1ms task, which is hopefully fast
    // enough for critical events. Debouncing pins is also implemented here.
    uint8_t limits = Limits_GetState();
    if(limits) {
        // X-Y-Z Limit
        if((DebounceCounterLimits == 0) && settings.system_flags & BITFLAG_ENABLE_LIMITS) {
            DebounceCounterLimits = 20;
            Limit_PinChangeISR();
        }
    }
    uint8_t controls = System_GetControlState();
    if(controls) {
        // System control
        if(DebounceCounterControl == 0) {
            DebounceCounterControl = 20;
            System_PinChangeISR();
        }
    }
    if(DebounceCounterLimits && !limits) {
        DebounceCounterLimits--;
    }
    if(DebounceCounterControl && !controls) {
        DebounceCounterControl--;
    }
    gMillis++;
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
         Stepper_MainISR();
        __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_CC1);
    }
    else if (__HAL_TIM_GET_FLAG(&htim9, TIM_FLAG_UPDATE) != RESET) {
        // OVF
        Stepper_PortResetISR();
        __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
    }
}


/**
  * @brief This function handles USART2 global interrupt.
  */
void
USART2_IRQHandler(void) {
    unsigned char c;
    if(__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET) {
        // The receive data register is not empty: Read one byte from it
        HAL_UART_Receive(&huart2, &c, 1, 0);
        ProcessReceive(c);
    }

    // If overrun condition occurs, clear the ORE flag and recover communication
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET) {
        HAL_UART_Receive(&huart2, &c, 1, 0);
    }
}
