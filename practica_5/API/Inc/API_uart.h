/*
 * API_uart.h
 *
 *  Created on: Apr 3, 2025
 *      Author: Mariano Deville
 */

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include "main.h"
#include "compatibility.h"

/**
 * Macros
 */
/* Comandos ASCII ----------------------------------------------------------*/
#define CLEAR_SCREEN_AND_HOME	"\e[2J\e[H"
#define CRLF					"\r\n"

/* Definiciones de tamaños y velocidad -------------------------------------*/
#define MAX_TX_BUFFER			128
#define RX_BUFFER_SIZE			20
#define BAUDE_RATE				115200

/**
 * Estructura con la información del puerto.
 */
typedef struct {

	UART_HandleTypeDef * puerto;
	bool_t mensaje_recibido;
	uint8_t rx_buff[RX_BUFFER_SIZE];
}puerto_UART;

/**
 * Respuestas de las funciones de envío de datos.
 */
typedef enum {

	OK,
	BUFFER_VACIO,
	SIN_DATOS,
	ERROR_SIZE,
	ERROR_TRANSMISION,
}Estado_TX_RX;

/**
 * Prototipo de funciones públicas.
 */
bool_t uartInit(puerto_UART * data_port, UART_HandleTypeDef * huart);
Estado_TX_RX uartSendString(puerto_UART * data_port, char * pstring);
Estado_TX_RX uartSendStringSize(puerto_UART * data_port, char * pstring, uint16_t size);
void uartReceiveStringSize(puerto_UART * data_port, uint16_t size);
bool_t isNewMessage(puerto_UART * data_port);
bool_t muestroConfiguracion(puerto_UART * data_port);

#endif /* API_INC_API_UART_H_ */
