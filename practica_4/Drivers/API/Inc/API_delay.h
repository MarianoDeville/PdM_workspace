/*
 * delay_no_bloqueante.h
 *
 *  Created on: Mar 20, 2025
 *      Author: Mariano Deville
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * Estructura defina para el manejo del delay no bloqueante.
 * Está definida en el header para poder utilizarla en cualquier parte del proyecto.
 */
typedef struct{

	tick_t startTime;
	tick_t duration;
	bool_t running;
} delay_t;

/**
 * Prototipo de funciones públicas.
 */
void DelayInit(delay_t * delay, tick_t duration);
bool_t DelayRead(delay_t * delay );
void DelayWrite(delay_t * delay, tick_t duration);

#endif /* API_INC_API_DELAY_H_ */
