/**
******************************************************************************
 * @file    API_debounce.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación de antirrebote para pulsador con delay no bloqueante.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#include "API_debounce.h"

/**
 * Macros
 */
#define TIEMPO_ANTI_REBOTE	40

/**
  * @brief  Inicializa las variables de la máuina de estado para la función anti rebote.
  * @param  Puntero a la estructura que contiene las variabes.
  * @retval None
  */
void DebounceFSMInit(debounceData_t * antirrebote_boton_n) {

	assert_param(antirrebote_boton_n);
	antirrebote_boton_n->estadoActual = BUTTON_UP;
	DelayInit(&antirrebote_boton_n->delay_anti_rebote, TIEMPO_ANTI_REBOTE);
}

/**
  * @brief  Utilizando una máquina de estados compruebo si hubo un cambio con el estado anterior.
  * @param  Puntero a la estructura que contiene las variabes.
  * @param  Estado actual - booleano.
  * @retval Devuelvo la información si el botón fue presionado, soltado, sin cambios o si hubo un error.
  * @note	En el enunciado la función no posee parámetros, pero de esta forma independizo totalmente
  * 		el driver del hardware en donde lo implemento.
  */
estadoPulsador_t DebounceFSMUpdate(debounceData_t * antirrebote_boton_n, bool_t estado_pin) {

	assert_param(antirrebote_boton_n);
	switch(antirrebote_boton_n->estadoActual) {

		case BUTTON_UP:

			if(!estado_pin)
				antirrebote_boton_n->estadoActual = BUTTON_FALLING;
			break;

		case BUTTON_FALLING:

			if(DelayRead(&antirrebote_boton_n->delay_anti_rebote)) {

				if(!estado_pin) {

					antirrebote_boton_n->estadoActual = BUTTON_DOWN;
					return PRESIONO_BOTON;
				} else {

					antirrebote_boton_n->estadoActual = BUTTON_UP;
					return RUIDO;
				}
			}
			break;

		case BUTTON_RAISING:

			if(DelayRead(&antirrebote_boton_n->delay_anti_rebote)) {

				if(estado_pin) {

					antirrebote_boton_n->estadoActual = BUTTON_UP;
					return SUELTO_BOTON;
				} else {

					antirrebote_boton_n->estadoActual = BUTTON_DOWN;
					return RUIDO;
				}
			}
			break;

		case BUTTON_DOWN:

			if(estado_pin)
				antirrebote_boton_n->estadoActual = BUTTON_RAISING;
			break;

		default:

			DebounceFSMInit(antirrebote_boton_n);
			return ERROR_ANTI_REBOTE;
	}
	return BOTON_SIN_CAMBIOS;
}

