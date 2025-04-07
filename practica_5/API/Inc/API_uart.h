/*
 * API_uart.h
 *
 *  Created on: Apr 3, 2025
 *      Author: mariano
 */

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include "main.h"
#include "compatibility.h"

#define CLEAR_SCREEN_AND_HOME	"\e[2J\e[H"
#define CRLF					"\r\n"

typedef enum {

	OK,
	BUFFER_VACIO,
	SIN_DATOS,
	ERROR_SIZE,
	ERROR_TRANSMISION,
}Estado_TX_RX;

bool_t uartInit();
Estado_TX_RX uartSendString(const uint8_t * pstring);
Estado_TX_RX uartSendStringSize(const uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);
bool_t isNewMessage(void);
bool_t muestroConfiguracion(void);

#endif /* API_INC_API_UART_H_ */
