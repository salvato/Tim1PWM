#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "Print.h"
#include "Config.h"
#include "usart.h"
#include "fifo_usart.h"
#include "Settings.h"
#include "Platform.h"
#include "System32.h"


#define MAX_BUFFER_SIZE     256


static char buf[512] = {0};
static uint16_t buf_idx = 0;


void
Print_Init(UART_HandleTypeDef* pUart) {
    Usart_Init(pUart, BAUD_RATE);
}


int
Printf(const char *str, ...) {
    char buffer[MAX_BUFFER_SIZE];
	uint8_t idx = 0;

	va_list vl;
	va_start(vl, str);
    int i = vsnprintf(buffer, MAX_BUFFER_SIZE, str, vl);
    if(i > MAX_BUFFER_SIZE) {
        i = MAX_BUFFER_SIZE;
    }
    for(uint8_t j = 0; j < i; j++) {
        buf[buf_idx++] = buffer[j];
    }
    va_end(vl);
    // Return number of sent bytes
	return idx;
}


int8_t
Getc(char *c) {
    if(FifoUsart_Get(USART_DIR_RX, c) == 0) {
		return 0;
	}
	return -1;
}


int
Putc(const char c) {
    buf[buf_idx++] = c;
	return 0;
}


void
Print_Flush(void) {
    Delay_ms(30);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    Usart_Write(&huart2, false, (uint8_t*)buf, (uint8_t)buf_idx);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    memset(buf, 0, sizeof(buf));
    buf_idx = 0;
}


// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up
// techniques are actually just slightly slower. Found this out the hard way.
void
PrintFloat(float n, uint8_t decimal_places) {
	if(n < 0) {
		Putc('-');
		n = -n;
	}

	uint8_t decimals = decimal_places;

	while(decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}

	if(decimals) {
		n *= 10;
	}
    n += 0.5f; // Add rounding factor. Ensures carryover through entire value.

	// Generate digits backwards and store in string.
	unsigned char buf[13];
	uint8_t i = 0;
    uint32_t a = (uint32_t)n;

	while(a > 0) {
		buf[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}

	while(i < decimal_places) {
		buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}

	if(i == decimal_places) { // Fill in leading zero, if needed.
		buf[i++] = '0';
	}

	// Print the generated string.
	for(; i > 0; i--) {
		if(i == decimal_places) {
			Putc('.');
		} // Insert decimal point in right place.
        Putc((char)buf[i-1]);
	}
}


// Floating value printing handlers for special variables types used in Grbl and are defined
// in the config.h.
//  - CoordValue: Handles all position or coordinate values in inches or mm reporting.
//  - RateValue: Handles feed rate and current velocity in inches or mm reporting.
void
PrintFloat_CoordValue(float n) {
	if(BIT_IS_TRUE(settings.flags, BITFLAG_REPORT_INCHES)) {
		PrintFloat(n*INCH_PER_MM,N_DECIMAL_COORDVALUE_INCH);
	}
	else {
		PrintFloat(n, N_DECIMAL_COORDVALUE_MM);
	}
}


void
PrintFloat_RateValue(float n) {
	if(BIT_IS_TRUE(settings.flags, BITFLAG_REPORT_INCHES)) {
		PrintFloat(n*INCH_PER_MM,N_DECIMAL_RATEVALUE_INCH);
	}
	else {
		PrintFloat(n, N_DECIMAL_RATEVALUE_MM);
	}
}
