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
  * @retval Devuelvo la información si el botón fue presionado, soltado, sin cambios o si hubo un error.
  * @note	En el enunciado la función no posee parámetros, pero de esta forma independizo totalmente
  * 		el driver del hardware en donde lo implemento.
  */
estadoPulsador_t debounceFSM_update(bool_t estado_pin) {

	switch(estadoActual) {

		case BUTTON_UP:

			if(!estado_pin) {

				DelayRead(&delay_anti_rebote);
				estadoActual = BUTTON_FALLING;
			}
			break;

		case BUTTON_FALLING:

			if(DelayRead(&delay_anti_rebote)) {

				if(!estado_pin) {

					estadoActual = BUTTON_DOWN;
					return PRESIONO_BOTON;
				} else {

					estadoActual = BUTTON_UP;
					return RUIDO;
				}
			}
			break;

		case BUTTON_RAISING:

			if(DelayRead(&delay_anti_rebote)) {

				if(estado_pin) {

					estadoActual = BUTTON_UP;
					return SUELTO_BOTON;
				} else {

					estadoActual = BUTTON_DOWN;
					return RUIDO;
				}
			}
			break;

		case BUTTON_DOWN:

			if(estado_pin) {

				DelayRead(&delay_anti_rebote);
				estadoActual = BUTTON_RAISING;
			}
			break;

		default:

			debounceFSM_init();
			return ERROR_ANTI_REBOTE;
	}
	return BOTON_SIN_CAMBIOS;
}

