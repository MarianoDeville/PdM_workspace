/*
 * API_uart.c
 *
 *  Created on: Apr 3, 2025
 *      Author: mariano
 */

#include "API_uart.h"
#include "string.h"


extern UART_HandleTypeDef huart2;

bool_t uartInit() {

	return false;
}

void uartSendString(uint8_t * pstring) {

	uint16_t tamano = strlen((const char *)&pstring);
	HAL_UART_Transmit(&huart2, pstring, tamano, HAL_MAX_DELAY);
}

void uartSendStringSize(uint8_t * pstring, uint16_t size) {

}

void uartReceiveStringSize(uint8_t * pstring, uint16_t size) {

}




