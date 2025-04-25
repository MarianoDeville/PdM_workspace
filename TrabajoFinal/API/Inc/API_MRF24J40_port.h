/**
 *******************************************************************************
 * @file    API_MRF24J40_port.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief	Archivo cabecera para el archivo API_MRF24J40_port.c
 *******************************************************************************
 * @attention Este archivo asegura la portabilidad del driver (API_MRF24J40.c)
 *
 *******************************************************************************
 */
#ifndef INC_API_MRF24J40_PORT_H_
#define INC_API_MRF24J40_PORT_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "drv_mspi_port.h"
//#include "../../redef_var.h"

#define _1_BYTE			(0x01)
#define _2_BYTES		(0x02)
#define SHIFT_BYTE		(0X08)
#define TIME_OUT_SPI	100


/* Prototipo de funciones públicas -------------------------------------------*/
void InicializoPines(void);
void SetCSPin(bool_t estado);
void SetWakePin(bool_t estado);
void SetResetPin(bool_t estado);
volatile bool_t IsMRF24Interrup(void);
void WriteByteSPIPort(uint8_t dato);
void Write2ByteSPIPort(uint16_t dato);
uint8_t ReadByteSPIPort(void);

#endif /* INC_API_MRF24J40_PORT_H_ */
