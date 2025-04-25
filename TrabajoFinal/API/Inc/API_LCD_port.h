/**
 *******************************************************************************
 * @file    API_LCD_port.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo API_LCD
 *******************************************************************************
 * @attention Este archivo asegura la portabilidad del driver (API_LCD.c)
 *
 *******************************************************************************
 */
#ifndef API_INC_API_LCD_port_H_
#define API_INC_API_LCD_port_H_

#include "main.h"

#define DelayLCD(x)	HAL_Delay(x)

void LCDWritePort(uint8_t dato);

#endif /* API_INC_API_LCD_port_H_ */
