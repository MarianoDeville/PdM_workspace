/**
******************************************************************************
 * @file    API_LCD.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver display.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#include "API_LCD.h"

/* Bits de control por hardware */
#define COMANDO				0x00
#define INFORMACION			0X01
#define ENABLE				0X04
#define BACK_LIGTH			0X08

/* Inicialización */
#define INICIALIZACION_1	0X30
#define INICIALIZACION_2	0X20

/* Control and display command */
#define CLEAR_DISPALY		0X01
#define RETURN_HOME			0X02
#define ENTRY_SET_MODE		0X04
#define INCREASE	 		0X02
#define DISPLAY_IS_SHIFTED	0X01
#define DISPLAY_ON_OFF      0X08
#define DISPLAY_ON 			0X04
#define CURSOR_ON 			0X02
#define BLINKING_ON			0X01
#define SET_FUNCTION		0X20
#define _8_BITS_INTERFACE	0X10
#define _2_LINE_DISPLAY		0X08
#define	_5x10_DOTS			0X04
#define SET_CG_RAM_ADDRESS	0X40
#define SET_DD_RAM_ADDRESS	0X80

#define HIGH_NIBBLE			0XF0
#define LOW_NIBBLE			4

#define _1_ms				1
#define _2_ms				2
#define _5_ms				5
#define _15_ms				15

/* Prototipo de funciones privadas -------------------------------------------*/
static void LCDWrite4Bits(char dato, uint8_t rs);
static void LCDWrite8Bits(char dato, uint8_t rs);

/* Funciones públicas --------------------------------------------------------*/
/**
  * @brief  Inicialización del display
  * @retval None
  */
void LCDInint(void) {

	DelayLCD(_15_ms);
	LCDWrite4Bits(INICIALIZACION_1, COMANDO);
	DelayLCD(_5_ms);
	LCDWrite4Bits(INICIALIZACION_1, COMANDO);
	DelayLCD(_1_ms);
	LCDWrite4Bits(INICIALIZACION_2, COMANDO);
	LCDWrite8Bits(DISPLAY_ON_OFF | DISPLAY_ON, COMANDO);
	LCDWrite8Bits(ENTRY_SET_MODE | INCREASE, COMANDO);
	LCDWrite8Bits(SET_FUNCTION | _2_LINE_DISPLAY, COMANDO);
	LCDClear();
	return;
}

/**
  * @brief  Escribo un caracter en el display.
  * @param  Caracter que voy a escribir.
  * @retval None.
  */
void LCDWriteCaracter(char caracter) {

	LCDWrite8Bits(caracter, INFORMACION);
}

/**
  * @brief	Escribo una cadena en el display.
  * @param  Puntero a la cadenas que voy a escribir.
  * @retval None.
  */
void LCDWriteString(char * cadena) {

	while(*cadena){

		LCDWriteCaracter(*cadena++);
	}
	return;
}

/**
  * @brief  Limpio la pantalla del display.
  * @retval None.
  */
void LCDClear(void) {

	LCDWrite8Bits(CLEAR_DISPALY, COMANDO);
	DelayLCD(_2_ms);
	return;
}

/**
  * @brief	Dejo el cursor en la posición indicada.
  * @param  Cantidad de caracteres desplazados desde el inicio de la primer linea.
  * @retval None.
  */
void LCDGoto(uint8_t pos) {

	LCDWrite8Bits(SET_DD_RAM_ADDRESS + pos, COMANDO);
	return;
}

/**
  * @brief  Escribo dos cadenas de caracteres, una en cada linea.
  * @param  Puntero a la primer cadena que voy a escribir.
  * @param  Puntero a la segunda cadena que voy a escribir.
  * @retval None.
  */
void LCDWrite2String(char *linea1, char *llinea2) {

	LCDGoto(COMIENZO_1_LINEA);
	LCDWriteString(linea1);
	LCDGoto(COMIENZO_2_LINEA);
	LCDWriteString(llinea2);
	return;
}

/**
  * @brief  Borro el contenido de una linea del display.
  * @param  Cantidad de caracteres desplazados desde el inicio de la primer linea.
  * @retval None.
  */
void LCDClearLinea(uint8_t pos) {

	LCDGoto(pos);
	for(uint8_t i = 0; i < 40; i++) {

		LCDWriteCaracter(' ');
	}
}

/** Funciones privadas
 *
 */
/**
  * @brief	Escribo 8 bits por el puerto destinado al display de 4 bits por vez.
  * @param  Dato que escribo, 1 byte.
  * @param  Indicador si es un comando lo que se escribe o información, 1 byte.
  * @retval None.
  */
static void LCDWrite4Bits(char dato, uint8_t rs) {

	LCDWritePort(dato | rs | ENABLE | BACK_LIGTH);
	LCDWritePort(dato | rs | BACK_LIGTH);
	return;
}

/**
  * @brief	Escribo 8 bits por el puerto destinado al display.
  * @param  Dato que escribo, 1 byte.
  * @param  Indicador si es un comando lo que se escribe o información, 1 byte.
  * @retval None.
  */
static void LCDWrite8Bits(char dato, uint8_t rs) {

	LCDWrite4Bits(dato & HIGH_NIBBLE, rs);
	LCDWrite4Bits(dato << LOW_NIBBLE, rs);
	return;
}

