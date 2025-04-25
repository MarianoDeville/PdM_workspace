/**
******************************************************************************
 * @file    API_debounce.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Archivo cabecera para la implementación driver antirrebote
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_delay.h"

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
 * Respuestas de la máquina de estados.
 */
typedef enum {

	ERROR_ANTI_REBOTE,
	PRESIONO_BOTON,
	SUELTO_BOTON,
	BOTON_SIN_CAMBIOS,
	RUIDO,
} estadoPulsador_t;

/**
 * Estructura definida para el manejo del antirrebote.
 */
typedef struct {

	debounceState_t estadoActual;
	delayNoBloqueanteData delay_anti_rebote;
} debounceData_t;

/**
 * Prototipo de funciones públicas.
 */
void DebounceFSMInit(debounceData_t * antirrebote_boton_n);
estadoPulsador_t DebounceFSMUpdate(debounceData_t * antirrebote_boton_n, bool_t estado_pin);

#endif /* API_INC_API_DEBOUNCE_H_ */
