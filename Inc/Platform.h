#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"


//---- I2C ----//
#define EEPROM_I2C			I2C1


//---- SPI ----//
#define SPI_W5500			SPI3


//---- USART ----//

// USART used for Printf(...)
#define STDOUT				USART2


//---- Defines ----//

// Direction definitions
#define FIFO_DIR_RX		    0
#define FIFO_DIR_TX		    1


// Communication interface
// Comment out to use serial interface
//#define ETH_IF

#define ETH_SOCK            0
#define ETH_PORT            30501


#endif /* PLATFORM_H_INCLUDED */
