/*
 * API_debounce.h
 *
 *  Created on: Mar 28, 2025
 *      Author: mariano
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_delay.h"


typedef enum {

	ERROR_ANTI_REBOTE,
	PRESIONO_BOTON,
	SUELTO_BOTON,
	BOTON_SIN_CAMBIOS,
	RUIDO,
} estadoPulsador_t;

/**
 * Prototipo de funciones públicas.
 */
void debounceFSM_init(void);
estadoPulsador_t debounceFSM_update(bool_t estado_pin);

#endif /* API_INC_API_DEBOUNCE_H_ */
