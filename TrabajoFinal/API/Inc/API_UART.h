/**
******************************************************************************
 * @file    API_UART.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Archivo cabecera para la implementación driver puerto UART con
 * 			interrupciones
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
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
#define RETORNO_CARRO			'\r'
/* Definiciones de tamaños y velocidad -------------------------------------*/
#define MAX_TX_BUFFER_UART		128
#define	RX_MSG_SIZE				1
#define RX_MSG_LAST_POS			(RX_MSG_SIZE - 1)
#define BAUDE_RATE_UART			115200

/**
 * Estructura con la información del puerto.
 */
typedef struct {

	UART_HandleTypeDef * puerto;
	bool_t mensaje_recibido;
	uint8_t rx_buff[RX_MSG_SIZE];
}puerto_UART;

/**
 * Respuestas de la máquina de estados.
 */
typedef enum {

	COMIENZO_MSG,
	NUEVO_CARACTER,
	FINAL_MSG,
	SIN_MSG,
	ESCUCHANDO,
	BUFFER_LLENO,
	MSG_ERROR
} estadoComunicacionUART_t;

/**
 * Estados posibles de la máquina de estados.
 */
typedef enum {
	CADENA_VACIA,
	ESPERO,
} estadoMsgUART_t;

/**
 * Estructura definida para el manejo de mensajes entrantes.
 */
typedef struct {

	puerto_UART * puertoUsado;
	estadoMsgUART_t estadoFSM;
	char * buffer;
	uint8_t bufferSize;
} datosFSMUART_t;

/**
 * Prototipo de funciones públicas.
 */
bool_t UARTtInit(puerto_UART * data_port, UART_HandleTypeDef * huart);
estado_TX_RX_t UARTSendString(puerto_UART * data_port, const uint8_t * pstring);
estado_TX_RX_t UARTSendStringSize(puerto_UART * data_port, const uint8_t * pstring, uint16_t size);
void UARTReceiveStringSize(puerto_UART * data_port, uint16_t size);
volatile bool_t UARTIsNewMessage(puerto_UART * data_port);
bool_t PutConfiguration(puerto_UART * data_port);
void UARTFSMInit(datosFSMUART_t * datosFSM, puerto_UART * data_port, char * bufferEntrada, uint8_t bufferSize);
estadoComunicacionUART_t CheckMsgUART(datosFSMUART_t * datosFSM);

#endif /* API_INC_API_UART_H_ */
