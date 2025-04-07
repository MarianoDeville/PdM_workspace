/*
 * API_uart.c
 *
 *  Created on: Apr 3, 2025
 *      Author: mariano
 */

#include "API_uart.h"
#include "string.h"

extern UART_HandleTypeDef huart5;
static bool_t nuevo_mensaje;

bool_t uartInit() {

	nuevo_mensaje = false;
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart5) != HAL_OK)
	    return false;

	if(!muestroConfiguracion())
	    return false;
	return true;
}

Estado_TX_RX uartSendString(const uint8_t * pstring) {

	if(pstring == NULL)
		return BUFFER_VACIO;
	uint16_t size = strlen((const char *)pstring);

	if(size == 0)
		return SIN_DATOS;

	if(HAL_UART_Transmit(&huart5, pstring, size, HAL_MAX_DELAY) != HAL_OK)
		return ERROR_TRANSMISION;
	return OK;
}

Estado_TX_RX uartSendStringSize(const uint8_t * pstring, uint16_t size) {

	if(pstring == NULL)
			return BUFFER_VACIO;
	if(size == 0)
		return SIN_DATOS;

	if(size > sizeof(pstring))
		return ERROR_SIZE;

	if(HAL_UART_Transmit(&huart5, pstring, size, HAL_MAX_DELAY) != HAL_OK)
		return ERROR_TRANSMISION;
	return OK;
}

void uartReceiveStringSize(uint8_t * pstring, uint16_t size) {

	HAL_UART_Receive_IT(&huart5, pstring, size);
	nuevo_mensaje = true;
	return;
}

bool_t isNewMessage(void) {

	if(nuevo_mensaje) {

		nuevo_mensaje = false;
		return true;
	}
	return false;
}

bool_t muestroConfiguracion(void) {

	if(uartSendString(CLEAR_SCREEN_AND_HOME
						"Puerto: UART5" CRLF
						"Baud rate: 115200" CRLF
						"Palabra de dato: 8 bits" CRLF
						"Paridad: sin" CRLF
						"Stop bit: 1\n" CRLF) != OK)
		return false;
	return true;
}



