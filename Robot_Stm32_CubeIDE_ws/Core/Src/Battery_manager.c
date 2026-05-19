/*
 * Battery_manager.c
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */


/*
 * battery_manager.c
 *
 *  Created on: Dec 8, 2025
 *      Author: jaja
 */


#include "Battery_manager.h"
#include "app_config.h"
#include "Neopixel.h"  // <--- AJOUT IMPORTANT : Inclusion du driver LED
extern TIM_HandleTypeDef htim4;


void Battery_Init(Battery_t *bat, ADC_HandleTypeDef *hadc) {


    bat->hAdc = hadc;
    bat->voltage = 0.0f;
    bat->state = BAT_CRITICAL;

    // Calibration ADC pour précision
    HAL_ADCEx_Calibration_Start(bat->hAdc, ADC_SINGLE_ENDED);

    // Initialisation du ruban LED (buffer à 0)
    WS2812_Init();
}

void Battery_Update(Battery_t *bat) {
    // Lecture ADC Polling
    HAL_ADC_Start(bat->hAdc);
    if (HAL_ADC_PollForConversion(bat->hAdc, 10) == HAL_OK) {
        uint32_t raw = HAL_ADC_GetValue(bat->hAdc);

        // Conversion en Volts
        bat->voltage = (float)raw * BATTERY_CONVERSION_COEFF;

        // Machine à état simple (Hystérésis recommandée pour éviter le scintillement aux limites)
        if (bat->voltage > BAT_LEVEL_FULL) bat->state = BAT_FULL;
        else if (bat->voltage > BAT_LEVEL_MID) bat->state = BAT_MEDIUM;
        else if (bat->voltage > BAT_LEVEL_LOW) bat->state = BAT_LOW;
        else bat->state = BAT_CRITICAL;
    }
    HAL_ADC_Stop(bat->hAdc);
}

void Battery_UpdateLEDs(Battery_t *bat) {
    // 1. LED Standard (Toggle pour heartbeat - Debug visuel simple)


    // 2. NeoPixel Logic
    // Note : J'utilise une intensité max de 50 (au lieu de 255) pour ménager vos yeux et le courant USB.

    switch (bat->state) {
        case BAT_FULL:
            // Vert
            WS2812_SetColor(0, 50, 0);
            break;

        case BAT_MEDIUM:
            // Orange (Rouge + un peu de Vert)
        	//HAL_GPIO_TogglePin(LED_2_GPIO_PortHAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);, LED_2_Pin);
            WS2812_SetColor(50, 25, 0);
            break;

        case BAT_LOW:
            // Rouge
        	HAL_GPIO_TogglePin(LED_1_GPIO_Port,LED_1_Pin);
        	HAL_GPIO_TogglePin(LED_2_GPIO_Port,LED_2_Pin);
            WS2812_SetColor(50, 0, 0);
            break;

        case BAT_CRITICAL:
            // Clignotement Rouge Rapide (Non-bloquant)
            // Utilise HAL_GetTick() pour diviser le temps.
            // 200ms allumé, 200ms éteint.
            if ((HAL_GetTick() / 200) % 2 == 0) {
                WS2812_SetColor(50, 0, 0); // Rouge
            } else {
                WS2812_SetColor(0, 0, 0);  // Éteint
            }
            break;
    }

    // Envoi effectif des données au ruban LED
    WS2812_Refresh();
}
