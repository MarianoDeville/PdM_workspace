/**
******************************************************************************
 * @file    API_LCD.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Archivo cabecera para la implementación driver display con
 * 			comunicacion I2C
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */
#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include "API_LCD_port.h"

#define COMIENZO_1_LINEA	(0X00)
#define COMIENZO_2_LINEA	(0X40)


void LCDInint(void);
void LCDWriteCaracter(char caracter);
void LCDWriteString(char * cadena);
void LCDClear(void);
void LCDGoto(uint8_t pos);
void LCDWrite2String(char *linea1, char *llinea2);
void LCDClearLinea(uint8_t pos);


#endif /* API_INC_API_LCD_H_ */
