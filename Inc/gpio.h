#pragma once

#include <stdbool.h>
#include "stm32f4xx_hal.h"

// GPIO Pins
// Stepper Pins
#define GPIO_STEP_X_PORT		GPIOA
#define GPIO_STEP_X_PIN			GPIO_PIN_10
#define GPIO_STEP_Y_PORT		GPIOB
#define GPIO_STEP_Y_PIN			GPIO_PIN_3
#define GPIO_STEP_Z_PORT		GPIOB
#define GPIO_STEP_Z_PIN			GPIO_PIN_5

// Direction Pins
#define GPIO_DIR_X_PORT			GPIOB
#define GPIO_DIR_X_PIN			GPIO_PIN_4
#define GPIO_DIR_Y_PORT			GPIOB
#define GPIO_DIR_Y_PIN			GPIO_PIN_10
#define GPIO_DIR_Z_PORT			GPIOA
#define GPIO_DIR_Z_PIN			GPIO_PIN_8

// Stepper Enable
#define GPIO_ENABLE_PORT		GPIOA
#define GPIO_ENABLE_PIN			GPIO_PIN_9

// Limit Pins
#define GPIO_LIM_X_PORT			GPIOC
#define GPIO_LIM_X_PIN			GPIO_PIN_7
#define GPIO_LIM_Y_PORT			GPIOB
#define GPIO_LIM_Y_PIN			GPIO_PIN_6
#define GPIO_LIM_Z_PORT			GPIOA
#define GPIO_LIM_Z_PIN			GPIO_PIN_6

// Spindle
#define GPIO_SPINDLE_PWM_PORT	    GPIOA
#define GPIO_SPINDLE_PWM_PIN	    GPIO_PIN_7
#define GPIO_SPINDLE_PWM_SOURCE_PIN	GPIO_PinSource7
#define GPIO_SPINDLE_DIR_PORT	    GPIOA
#define GPIO_SPINDLE_DIR_PIN	    GPIO_PIN_5
#define GPIO_SPINDLE_ENA_PORT	    GPIOB
#define GPIO_SPINDLE_ENA_PIN	    GPIO_PIN_9

// Safety door
#define GPIO_DOOR_PORT			GPIOB
#define GPIO_DOOR_PIN			GPIO_PIN_8

// Control pins
#define GPIO_CTRL_RST_PORT		GPIOA
#define GPIO_CTRL_RST_PIN		GPIO_PIN_0
#define GPIO_CTRL_FEED_PORT		GPIOA
#define GPIO_CTRL_FEED_PIN		GPIO_PIN_1
#define GPIO_CTRL_START_PORT	GPIOA
#define GPIO_CTRL_START_PIN		GPIO_PIN_4

// Coolant
#define GPIO_COOL_FLOOD_PORT	GPIOB
#define GPIO_COOL_FLOOD_PIN		GPIO_PIN_0
#define GPIO_COOL_MIST_PORT		GPIOC
#define GPIO_COOL_MIST_PIN		GPIO_PIN_1

// Probe
#define GPIO_PROBE_PORT			GPIOC
#define GPIO_PROBE_PIN			GPIO_PIN_0


#define GPIO_STEPPER	0
#define GPIO_PROBE		1
#define GPIO_SPINDLE	2
#define GPIO_LIMIT		3
#define GPIO_COOLANT	4
#define GPIO_SYSTEM		5


#ifdef __cplusplus
extern "C" {
#endif


void GPIO_InitGPIO(char gpio);

void GPIO_InitStepper(void);
void GPIO_InitProbe(void);
void GPIO_InitSpindle(void);
void GPIO_InitLimit(void);
void GPIO_InitCoolant(void);
void GPIO_InitSystem(void);


#ifdef __cplusplus
}
#endif

