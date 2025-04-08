/*
 * API_uart.c
 *
 *  Created on: Apr 3, 2025
 *      Author: Mariano Deville
 */
#include "API_uart.h"
#include "string.h"

/**
  * @brief  Inicialización del puerto serie.
  * @param  Puntero a la estructura que contiene las variabes del puerto seleccionado.
  * @retval Devuelvo verdadero si la inicialización se realizo sin problemas.
  */
bool_t uartInit(puerto_UART * data_port, UART_HandleTypeDef * huart) {

	data_port->mensaje_recibido = false;
	data_port->puerto = huart;
	data_port->puerto->Instance = UART5;
	data_port->puerto->Init.BaudRate = BAUDE_RATE;
	data_port->puerto->Init.WordLength = UART_WORDLENGTH_8B;
	data_port->puerto->Init.StopBits = UART_STOPBITS_1;
	data_port->puerto->Init.Parity = UART_PARITY_NONE;
	data_port->puerto->Init.Mode = UART_MODE_TX_RX;
	data_port->puerto->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	data_port->puerto->Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(data_port->puerto) != HAL_OK)
	    return false;

	if(!muestroConfiguracion(data_port))
	    return false;
	return true;
}

/**
  * @brief  Envío por el puerto una cadena de caracteres.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero a la cadena que se desea enviar.
  * @retval Devuelvo la información del resultado de la transmisión
  */
Estado_TX_RX uartSendString(puerto_UART * data_port, char * pstring) {

	if(pstring == NULL)
		return BUFFER_VACIO;
	uint16_t size = strlen((char *)pstring);
	uartSendStringSize(data_port, pstring, size);
	return OK;
}

/**
  * @brief  Envío por el puerto una cadena de caracteres.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero a la cadena que se desea enviar.
  * @param  Tamaño de la cadena a enviar.
  * @retval Devuelvo la información del resultado de la transmisión.
  */
Estado_TX_RX uartSendStringSize(puerto_UART * data_port, char * pstring, uint16_t size) {

	if(pstring == NULL)
			return BUFFER_VACIO;
	if(size == 0)
		return SIN_DATOS;

	if(size > MAX_TX_BUFFER)
		return ERROR_SIZE;

	if(HAL_UART_Transmit(data_port->puerto, (uint8_t *)pstring, size, HAL_MAX_DELAY) != HAL_OK)
		return ERROR_TRANSMISION;
	return OK;
}

/**
  * @brief  Recibo un mensaje entrante.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero a la cadena que se usa como buffer de entrada.
  * @param  Tamaño de la cadena que se espera entre.
  * @retval None.
  */
void uartReceiveStringSize(puerto_UART * data_port, uint16_t size) {

	HAL_UART_Receive_IT(data_port->puerto, data_port->rx_buff, size);
	data_port->mensaje_recibido = true;
	return;
}

/**
  * @brief  Consulto si ha llegado un mensaje por el puerto.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @retval Booleano indicando la llegada.
  */
bool_t isNewMessage(puerto_UART * data_port) {

	if(data_port->mensaje_recibido) {

		data_port->mensaje_recibido = false;
		return true;
	}
	return false;
}

/**
  * @brief  Envio por el puerto comandos ASCII para limpiar la pantalla e imprimir información de la conección.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @retval Devuelvo la información del resultado de la transmisión.
  */
bool_t muestroConfiguracion(puerto_UART * data_port) {

	if(uartSendString(data_port, CLEAR_SCREEN_AND_HOME
							"Puerto: UART5" CRLF
							"Baud rate: 115200" CRLF
							"Palabra de dato: 8 bits" CRLF
							"Paridad: sin" CRLF
							"Stop bit: 1\n" CRLF) != OK)
		return false;
	return true;
}



