/*
 * Neopixel.c
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */


/*
 * ws2812.c
 *
 *  Created on: Dec 8, 2025
 *      Author: jaja
 */

#include "Neopixel.h"

// Lien vers le handle TIM15 défini dans main.c
extern TIM_HandleTypeDef htim4;

#define WS2812_TIMER              htim4
#define WS2812_TIMER_CHANNEL      TIM_CHANNEL_4

// --- Timings pour STM32G4 à 170 MHz (ARR = 212) ---
// '0' logique : ~32% de la période -> 0.32 * 212 = 68
// '1' logique : ~64% de la période -> 0.64 * 212 = 136
#define WS2812_DUTY_LOW           6
#define WS2812_DUTY_HIGH          13

#define WS2812_BITS_PER_LED       24
#define WS2812_RESET_SLOTS        50 // Temps de pause à la fin

#define WS2812_BUF_LEN  (WS2812_NUM_LEDS * WS2812_BITS_PER_LED + WS2812_RESET_SLOTS)

// Buffer DMA en uint16_t car la valeur max est 212 (> 255 serait impossible sur 8 bits)
static uint16_t ws2812_buf[WS2812_BUF_LEN];
static uint8_t  ws2812_busy = 0;

void WS2812_Init(void)
{
    // Initialisation du buffer à 0 (LED éteinte)
    for (int i = 0; i < WS2812_BUF_LEN; i++) {
        ws2812_buf[i] = 0;
    }
}

// Définir la couleur (Format GRB pour WS2812)
void WS2812_SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t idx = 0;
    uint8_t colors[3] = { g, r, b }; // Ordre GRB

    for (int c = 0; c < 3; c++) {
        for (int bit = 7; bit >= 0; bit--) {
            if (colors[c] & (1 << bit)) {
                ws2812_buf[idx++] = WS2812_DUTY_HIGH;
            } else {
                ws2812_buf[idx++] = WS2812_DUTY_LOW;
            }
        }
    }
    // Note: Les slots de RESET à la fin restent à 0 par défaut
}

// Envoyer les données via DMA
void WS2812_Refresh(void)
{
    if (ws2812_busy) return;

    ws2812_busy = 1;

    // Démarrage du transfert DMA
    if (HAL_TIM_PWM_Start_DMA(&WS2812_TIMER, WS2812_TIMER_CHANNEL, (uint32_t *)ws2812_buf, WS2812_BUF_LEN) != HAL_OK) {
        ws2812_busy = 0; // En cas d'erreur, on libère le flag
    }
}

// Callback appelé automatiquement quand le DMA a fini d'envoyer le buffer
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4) {
        HAL_TIM_PWM_Stop_DMA(&WS2812_TIMER, WS2812_TIMER_CHANNEL);
        ws2812_busy = 0;
    }
}

