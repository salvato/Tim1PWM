#include "main.h"
#include "tim.h"
#include "fifo_usart.h"
#include "usart.h"
#include "CoolantControl.h"
#include "GCode.h"
#include "Limits.h"
#include "MotionControl.h"
#include "ToolChange.h"
#include "Print.h"
#include "Probe.h"
#include "Protocol.h"
#include "Report.h"
#include "SpindleControl.h"
#include "System.h"
#include "Stepper.h"
#include "Settings.h"
#include "System32.h"


static void SystemClock_Config(void);
static void MX_GPIO_Init(void);


TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim9;
UART_HandleTypeDef huart2;
TIM_OC_InitTypeDef sConfigOC;


// Declare system global variable structure
System_t sys;
int32_t sys_position[N_AXIS];                    // Real-time machine (aka home) position vector in steps.
int32_t sys_probe_position[N_AXIS];              // Last probe position in machine coordinates and steps.
volatile uint8_t sys_probe_state;                // Probing state value.  Used to coordinate the probing cycle with stepper ISR.
volatile uint16_t sys_rt_exec_state;             // Global realtime executor bitflag variable for state management. See EXEC bitmasks.
volatile uint8_t sys_rt_exec_alarm;              // Global realtime executor bitflag variable for setting various alarms.
volatile uint8_t sys_rt_exec_motion_override;    // Global realtime executor bitflag variable for motion-based overrides.
volatile uint8_t sys_rt_exec_accessory_override; // Global realtime executor bitflag variable for spindle/coolant overrides.


int
main(void) {
//    HAL_Init() is used to initialize the HAL Library; it must be the first
//      *         instruction to be executed in the main program, it performs the following:
//      *           Configure the Flash prefetch, instruction and Data caches.
//      *           Configures the SysTick to generate an interrupt each 1 millisecond,
//      *           which is clocked by the HSI (at this stage, the clock is not yet
//      *           configured and thus the system is running from the internal HSI at 16 MHz).
//      *           Set NVIC Group Priority to 4.
//      *           Calls the HAL_MspInit() callback function defined in user file
//      *           "stm32f4xx_hal_msp.c" to do the global low level hardware initialization
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();// Initialize at first some peripherals for error signaling

    // Init formatted output
    Print_Init(&huart2);   // Initialize USART2 & enable the Receive interrupt

    System_Init();  // Initialize Some Control Inputs
    Stepper_Init(); // Configure step and direction interface pins & Timer9 (Used for Stepper Interrupt)
    Settings_Init();// Initialize the config subsystem
    System_ResetPosition();// Clear machine position.
    if(BIT_IS_TRUE(settings.flags, BITFLAG_HOMING_ENABLE)) {
        sys.state = STATE_ALARM;
    }
    else {
        sys.state = STATE_IDLE;
    }

    // Grbl-Advanced initialization loop upon power-up or a system abort.
    // For the latter, all processes will return to this loop to be cleanly
    // re-initialized.
    while(1) {
        // Reset system variables.
        uint16_t prior_state = sys.state;
        uint8_t home_state = sys.is_homed;
        System_Clear();           // Clear sys struct variable
        sys.state = prior_state;  // Restore previous system state
        sys.is_homed = home_state;// and homing state.

        Probe_Reset();// Clear probe position structure

        sys_probe_state = 0;// Used to coordinate the probing cycle with stepper ISR.
        sys_rt_exec_state = 0;// Realtime executor bitflag variable for state management. See EXEC bitmasks.
        sys_rt_exec_alarm = 0;// Realtime executor bitflag variable for setting various alarms.
        sys_rt_exec_motion_override = 0;// Realtime executor bitflag variable for motion-based overrides.
        sys_rt_exec_accessory_override = 0;// Realtime executor bitflag variable for spindle/coolant overrides.

        // Reset Grbl-Advanced primary systems.
        GC_Init(); // G-Code interpreter Init
        Planner_Init();
        MC_Init(); // Motion Control Init
        TC_Init(); // Tool Change Init

        Coolant_Init();
        Limits_Init();
        Probe_Init();
        Spindle_Init();
        Stepper_Reset();

        // Sync cleared gcode and planner positions to current system position.
        Planner_SyncPosition(); // Reset the planner position vectors.
        GC_SyncPosition(); // Sets g-code parser position in mm. Input in steps.

        // Print welcome message.
        // Indicates an initialization has occured at power-up or with a reset.
        Report_InitMessage();

        //-------------------------------------------------------------------//
        //-- Start Grbl-Advanced main loop.
        //-- Processes program inputs and executes them.
        Protocol_MainLoop(); // defined in Protocol.c

        // Clear serial buffer after soft reset to prevent undefined behavior
        FifoUsart_Init();
    }
}


void
SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }

    //Configure the Systick interrupt time
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    //Configure the Systick
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    // SysTick_IRQn interrupt configuration
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


static void
MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIO Ports Clock Enable
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure GPIO pin Output Level
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    // Configure GPIO pin : B1_Pin
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    // Configure GPIO pin : LD2_Pin
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}


void __attribute__ ((noreturn))
Error_Handler(void) {
    while(1) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(100);
    }
}


#ifdef  USE_FULL_ASSERT
void __attribute__ ((noreturn))
assert_failed(uint8_t *file, uint32_t line) {
    (void)file;
    (void)line;
    while(1) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(50);
    }
}
#endif /* USE_FULL_ASSERT */
