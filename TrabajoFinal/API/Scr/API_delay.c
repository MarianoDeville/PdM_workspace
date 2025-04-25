/**
******************************************************************************
 * @file    API_delay.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación de delay no bloqueante.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#include "API_delay.h"

/* Prototipo de funciones privadas -------------------------------------------*/
static bool_t DelayIsRunning(delayNoBloqueanteData * delay);

/**
  * @brief  Inicializa el delay no bloqueante.
  * @param  Puntero a la estructura con las variables.
  * @param  Duración del delay.
  * @retval None
  */
void DelayInit(delayNoBloqueanteData * delay, tick_t duration) {

	if(delay == NULL || duration == 0) {

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		return;
	}
	delay->duration = duration;
	delay->running = false;
	return;
}

/**
  * @brief  Obtengo el estado del delay.
  * @param  Puntero a la estructura con las variables.
  * @retval Estado del delay (contando / tiempo cumplido).
  */
bool_t DelayRead(delayNoBloqueanteData * delay) {

	if(delay == NULL) {

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		return false;
	}

	if(DelayIsRunning(delay) == false) {

		delay->startTime = HAL_GetTick();
		delay->running = true;
	} else {

		if((HAL_GetTick() - delay->startTime) >= delay->duration) {

			delay->running = false;
			return true;
		}
	}
	return false;
}

/**
  * @brief  Redefino el tiempo de delay.
  * @param  Puntero a la estructura con las variables.
  * @param  Duración del delay.
  * @retval None
  */
void DelayWrite(delayNoBloqueanteData * delay, tick_t duration) {

	if(delay == NULL || duration == 0) {

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		return;
	}
	delay->duration = duration;
	return;
}

/**
  * @brief  Reseteo la cuenta del tiempo.
  * @param  Puntero a la estructura con las variables.
  * @retval None
  */
void DelayReset( delayNoBloqueanteData * delay) {

	delay->startTime = HAL_GetTick();
	delay->running = true;
	return;
}

/**
  * @brief  Consulto el estado del delay.
  * @param  Puntero a la estructura con las variables.
  * @retval Estado del delay (contando / tiempo cumplido).
  * @note	En el trabajo práctico la define como función pública, pero para mejorar
  * 		el encapsulamiento la defino como privada ya que solo se utiliza en el
  * 		ámbito de este archivo.
  */
static bool_t DelayIsRunning(delayNoBloqueanteData * delay) {

	if(delay == NULL) {

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		return false;
	}
	return delay->running;
}
