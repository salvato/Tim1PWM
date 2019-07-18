/*
  FIFO_USART.c - FIFO USART Implementation
  Part of STM32F4_HAL

  Copyright (c)	2016-2017 Patrick F.

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

/* Very simple queue
 * These are FIFO queues which discard the new data when full.
 *
 * Queue is empty when in == out.
 * If in != out, then
 *  - items are placed into in before incrementing in
 *  - items are removed from out before incrementing out
 * Queue is full when in == (out-1 + QUEUE_SIZE) % QUEUE_SIZE;
 *
 * The queue will hold QUEUE_ELEMENTS number of items before the
 * calls to FifoUsart_Insert fail.
 */
#include <string.h>
#include "fifo_usart.h"
#include "debug.h"


static char FifoQueue[2][QUEUE_SIZE];
static uint8_t QueueIn[2];
static uint8_t QueueOut[2];
static uint32_t Count = 0;


void
FifoUsart_Init(void) {
    memset(QueueIn, 0, sizeof(QueueIn));
    memset(QueueOut, 0, sizeof(QueueOut));
}


int8_t
FifoUsart_Insert(uint8_t direction, char ch) {
    if(direction > 1) {
		d_printf("ERROR: USART direction out of range\n");
		return -1;
	}
    if(QueueIn[direction] == ((QueueOut[direction] - 1 + QUEUE_SIZE) % QUEUE_SIZE)) {
        return -1; // Queue Full
    }
    FifoQueue[direction][QueueIn[direction]] = ch;
    QueueIn[direction] = (QueueIn[direction] + 1) % QUEUE_SIZE;
    Count++;
    return 0; // No errors
}


int8_t
FifoUsart_Get(uint8_t direction, char *ch) {
	if(direction > 1) {
		d_printf("ERROR: USART direction out of range\n");
		return -1;
	}
    if(QueueIn[direction] == QueueOut[direction]) {
        return -1; /* Queue Empty - nothing to get*/
    }
    *ch = FifoQueue[direction][QueueOut[direction]];
    QueueOut[direction] = (QueueOut[direction] + 1) % QUEUE_SIZE;
    Count--;
    return 0; // No errors
}


uint32_t
FifoUsart_Available() {
    return (QUEUE_ELEMENTS - Count);
}
