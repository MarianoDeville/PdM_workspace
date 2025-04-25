/**
 *******************************************************************************
 * @file    API_LCD_port.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo API_LCD
 *******************************************************************************
 * @attention Este archivo asegura la portabilidad del driver (API_LCD.c)
 *
 *******************************************************************************
 */

#include "API_LCD_port.h"

#define LCD_DIR 0X27

extern I2C_HandleTypeDef hi2c3;

void LCDWritePort(uint8_t dato) {

	HAL_I2C_Master_Transmit (&hi2c3, LCD_DIR << 1 , &dato, sizeof(dato), HAL_MAX_DELAY);
}
