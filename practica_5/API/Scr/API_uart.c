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

	  huart2.Instance = USART2;
	  huart2.Init.BaudRate = 9600;
	  huart2.Init.WordLength = UART_WORDLENGTH_8B;
	  huart2.Init.StopBits = UART_STOPBITS_1;
	  huart2.Init.Parity = UART_PARITY_NONE;
	  huart2.Init.Mode = UART_MODE_TX_RX;
	  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	  if (HAL_UART_Init(&huart2) != HAL_OK)
	  {
	    Error_Handler();
	    return false;
	  }
	return true;
}

void uartSendString(uint8_t * pstring) {

//	uint16_t tamano = strlen((const char *)pstring);
	HAL_UART_Transmit(&huart2, pstring, 10, HAL_MAX_DELAY);
}

void uartSendStringSize(uint8_t * pstring, uint16_t size) {

}

void uartReceiveStringSize(uint8_t * pstring, uint16_t size) {

}




