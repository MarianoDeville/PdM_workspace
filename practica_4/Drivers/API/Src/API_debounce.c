/*
 * API_debounce.c
 *
 *  Created on: Mar 28, 2025
 *      Author: mariano
 */
#include "API_debounce.h"

/**
 * Macros
 */
#define TIEMPO_ANTI_REBOTE	40

/**
 * Estados posibles de la máquina de estados.
 */
typedef enum {

	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RAISING,
} debounceState_t;

/**
 * Variables privadas.
 */
static bool_t tecla_fue_presionada;
static debounceState_t estadoActual;
static delay_t delay_anti_rebote;

/**
 * Prototipo de funciones privadas.
 */
static void buttonPressed(void);
static void buttonReleased(void);

/**
  * @brief  Inicializa las variables de la máuina de estado para la función anti rebote.
  * @param  None.
  * @retval None
  */
void debounceFSM_init(void) {

	estadoActual = BUTTON_UP;
	DelayInit(&delay_anti_rebote, TIEMPO_ANTI_REBOTE);
	tecla_fue_presionada = false;
}

/**
  * @brief  Utilizando una máquina de estados compruebo si hubo un cambio con el estado anterior.
  * @param  Estado actual - booleano.
  * @retval None
  * @note	En el enunciado la función no posee parámetros, pero de esta forma independizo totalmente
  * 		el driver del hardware en donde lo implemento.
  */
void debounceFSM_update(bool_t estado_pin) {

	switch(estadoActual) {

		case BUTTON_UP:

			if(!estado_pin)
				estadoActual = BUTTON_FALLING;
			break;

		case BUTTON_FALLING:

			if(!estado_pin) {

				if(DelayRead(&delay_anti_rebote)) {

					estadoActual = BUTTON_DOWN;
					buttonPressed();
				} else {

					estadoActual = BUTTON_UP;
				}
			}
			break;

		case BUTTON_RAISING:

			if(estado_pin) {

				if(DelayRead(&delay_anti_rebote)) {

					estadoActual = BUTTON_UP;
					buttonReleased();
				} else {

					estadoActual = BUTTON_DOWN;
				}
			}
			break;

		case BUTTON_DOWN:

			if(estado_pin)
				estadoActual = BUTTON_RAISING;
			break;

		default:

			debounceFSM_init();
			return;
	}
}

/**
  * @brief  Compruebo si desde la última lectura se ha presionado el botón.
  * @param  None.
  * @retval Botón presionado - booleano.
  */
bool_t readKey(void) {

	if(!tecla_fue_presionada)
		return false;
	tecla_fue_presionada = false;
	return true;
}

/**
  * @brief  Dejo guardada en una variable si se presionó el botón.
  * @param  None.
  * @retval None
  */
static void buttonPressed(void) {

	tecla_fue_presionada = true;
	return;
}

/**
  * @brief  Dejo guardada en una variable si se soltó el botón.
  * @param  None.
  * @retval None
  */
static void buttonReleased(void) {

	tecla_fue_presionada = false;
	return;
}


