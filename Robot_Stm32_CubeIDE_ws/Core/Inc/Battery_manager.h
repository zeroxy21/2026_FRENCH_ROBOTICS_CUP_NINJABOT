/*
 * Battery_manager.h
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */

#ifndef INC_BATTERY_MANAGER_H_
#define INC_BATTERY_MANAGER_H_

#include "main.h"

// Enum pour l'état de charge (utile pour la LED)
typedef enum {
    BAT_CRITICAL,
    BAT_LOW,
    BAT_MEDIUM,
    BAT_FULL
} BatteryState_e;

typedef struct {
    ADC_HandleTypeDef *hAdc;
    float voltage;
    BatteryState_e state;
} Battery_t;

void Battery_Init(Battery_t *bat, ADC_HandleTypeDef *hadc);
void Battery_Update(Battery_t *bat);
void Battery_UpdateLEDs(Battery_t *bat); // Gère GPIO LED + (Futur) NeoPixel

#endif /* INC_BATTERY_MANAGER_H_ */
