/*
 * parpadeo.c
 *
 *  Created on: Mar 7, 2025
 *      Author: mariano
 */

#include "main.h"

void Parpadeo(int tiempo_ms){

	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	HAL_Delay(tiempo_ms);
}
