/*
 * API_debounce.h
 *
 *  Created on: Mar 28, 2025
 *      Author: mariano
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_delay.h"

/**
 * Prototipo de funciones públicas.
 */
void debounceFSM_init(void);
void debounceFSM_update(bool_t estado_pin);
bool_t readKey(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
