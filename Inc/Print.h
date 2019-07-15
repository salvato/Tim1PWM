#ifndef PRINT_H_INCLUDED
#define PRINT_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


void Print_Init(UART_HandleTypeDef* pUart);
int Printf(const unsigned char *str, ...);
void PrintFloat(float n, uint8_t decimal_places);
int8_t Getc(unsigned char *c);
int Putc(const unsigned char c);

void Print_Flush(void);

void PrintFloat_CoordValue(float n);
void PrintFloat_RateValue(float n);


#ifdef __cplusplus
}
#endif


#endif /* PRINT_H_INCLUDED */
