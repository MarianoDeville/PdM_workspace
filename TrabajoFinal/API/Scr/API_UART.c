/**
******************************************************************************
 * @file    API_UART.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver puerto UART con interrupciones para la lectura
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#include "API_UART.h"
#include "string.h"

#define TIME_OUT_UART	512

const uint8_t mensaje_inicial[] = CLEAR_SCREEN_AND_HOME
									"Puerto: UART5" CRLF
									"Baud rate: 115200" CRLF
									"Palabra de dato: 8 bits" CRLF
									"Paridad: sin" CRLF
									"Stop bit: 1\n" CRLF;

/**
  * @brief  Inicialización del puerto serie.
  * @param  Puntero a la estructura que contiene las variabes del puerto seleccionado.
  * @retval Devuelvo verdadero si la inicialización se realizo sin problemas.
  */
bool_t UARTtInit(puerto_UART * data_port, UART_HandleTypeDef * huart) {

	if(data_port == NULL || huart == NULL)
		Error_Handler();
	data_port->mensaje_recibido = false;
	data_port->puerto = huart;
	data_port->puerto->Instance = UART5;
	data_port->puerto->Init.BaudRate = BAUDE_RATE_UART;
	data_port->puerto->Init.WordLength = UART_WORDLENGTH_8B;
	data_port->puerto->Init.StopBits = UART_STOPBITS_1;
	data_port->puerto->Init.Parity = UART_PARITY_NONE;
	data_port->puerto->Init.Mode = UART_MODE_TX_RX;
	data_port->puerto->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	data_port->puerto->Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(data_port->puerto) != HAL_OK)
	    return false;
    return PutConfiguration(data_port);
}

/**
  * @brief  Envío por el puerto una cadena de caracteres.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero a la cadena que se desea enviar.
  * @retval Devuelvo la información del resultado de la transmisión
  */
estado_TX_RX_t UARTSendString(puerto_UART * data_port, const uint8_t * pstring) {

	if(data_port == NULL)
		Error_Handler();

	if(pstring == NULL)
		return BUFFER_VACIO;
	uint16_t size = strlen((char *)pstring);
	UARTSendStringSize(data_port, pstring, size);
	return OK;
}

/**
  * @brief  Envío por el puerto una cadena de caracteres.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero a la cadena que se desea enviar.
  * @param  Tamaño de la cadena a enviar.
  * @retval Devuelvo la información del resultado de la transmisión.
  */
estado_TX_RX_t UARTSendStringSize(puerto_UART * data_port, const uint8_t * pstring, uint16_t size) {

	if(data_port == NULL)
		Error_Handler();

	if(pstring == NULL)
			return BUFFER_VACIO;
	if(size == 0)
		return SIN_DATOS;

	if(size > MAX_TX_BUFFER_UART)
		return ERROR_SIZE;

	if(HAL_UART_Transmit(data_port->puerto, (uint8_t *)pstring, size, TIME_OUT_UART) != HAL_OK)
		return ERROR_TRANSMISION;
	return OK;
}

/**
  * @brief  Recibo un mensaje entrante.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Tamaño de la cadena que se espera entre.
  * @retval None.
  */
void UARTReceiveStringSize(puerto_UART * data_port, uint16_t size) {

	if(data_port == NULL)
		Error_Handler();
	HAL_UART_Receive_IT(data_port->puerto, data_port->rx_buff, size);
	data_port->mensaje_recibido = true;
	return;
}

/**
  * @brief  Consulto si ha llegado un mensaje por el puerto.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @retval Booleano indicando la llegada.
  */
volatile bool_t UARTIsNewMessage(puerto_UART * data_port) {

	if(data_port == NULL)
		Error_Handler();

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
bool_t PutConfiguration(puerto_UART * data_port) {

	if(data_port == NULL)
		Error_Handler();

	if(UARTSendString(data_port, mensaje_inicial) != OK)
		return false;
	return true;
}

/**
  * @brief	Inicializa las variables de la máuina de estado para la función anti rebote.
  * @param  Puntero a la estructura que contiene las variabes.
  * @param  Puntero a la estructura que contiene las variabes del puerto.
  * @param  Puntero al buffer de almacenamiento del mensaje entrante.
  * @retval
  */
void UARTFSMInit(datosFSMUART_t * datosFSM, puerto_UART * data_port, char * bufferEntrada, uint8_t bufferSize) {

	if(data_port == NULL || datosFSM == NULL)
		Error_Handler();
	datosFSM->puertoUsado = data_port;
	datosFSM->estadoFSM = CADENA_VACIA;
	datosFSM->buffer = bufferEntrada;
	datosFSM->bufferSize = bufferSize;
	memset(datosFSM->buffer, VACIO, sizeof(&datosFSM->buffer));
	data_port->mensaje_recibido = false;
}

/**
  * @brief	Utilizando una MEF compruebo el mensaje entrante, esperando un enter como
  * 		marca para el final de la cadena.
  * @param  Puntero a la estructura que contiene las variabes de la máquina de estados.
  * @retval	Devuelvo la información del llenado del buffer.
  */
estadoComunicacionUART_t CheckMsgUART(datosFSMUART_t * datosFSM) {

	if(datosFSM == NULL)
		Error_Handler();

	if(UARTIsNewMessage(datosFSM->puertoUsado)) {

		switch(datosFSM->estadoFSM) {

		case CADENA_VACIA:

			if(datosFSM->puertoUsado->rx_buff[RX_MSG_LAST_POS] == RETORNO_CARRO)
				return SIN_MSG;
			datosFSM->estadoFSM = ESPERO;

			if(strlen(datosFSM->buffer) == datosFSM->bufferSize)
				return BUFFER_LLENO;
			strcat(datosFSM->buffer, (char *)datosFSM->puertoUsado->rx_buff);
			return COMIENZO_MSG;

		case ESPERO:

			if(datosFSM->puertoUsado->rx_buff[RX_MSG_LAST_POS] == RETORNO_CARRO) {

				datosFSM->estadoFSM = CADENA_VACIA;
				return FINAL_MSG;
			}

			if(strlen(datosFSM->buffer) == datosFSM->bufferSize)
					return BUFFER_LLENO;
			strcat(datosFSM->buffer, (char *)datosFSM->puertoUsado->rx_buff);
			return NUEVO_CARACTER;

		default:

			datosFSM->estadoFSM = CADENA_VACIA;
			return MSG_ERROR;
		}
	}
	return ESCUCHANDO;
}
