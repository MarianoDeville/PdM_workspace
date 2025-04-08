/*
 * API_debounce.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Mariano Deville
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
 * Estructura defina para el manejo del antirrebote.
 * Está definida en el header para poder utilizarla en cualquier parte del proyecto.
 */
typedef struct {

	bool_t tecla_fue_presionada;
	debounceState_t estadoActual;
	delay_t delay_anti_rebote;
} debounceData_t;

/**
 * Prototipo de funciones públicas.
 */
void debounceFSM_init(debounceData_t * antirrebote_boton_n);
estadoPulsador_t debounceFSM_update(debounceData_t * antirrebote_boton_n, bool_t estado_pin);

#endif /* API_INC_API_DEBOUNCE_H_ */
