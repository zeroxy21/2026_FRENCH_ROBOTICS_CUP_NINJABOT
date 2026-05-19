/*
 * Neopixel.h
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */

#ifndef INC_NEOPIXEL_H_
#define INC_NEOPIXEL_H_

#include "main.h"

// Nombre de LEDs sur votre ruban
#define WS2812_NUM_LEDS    1

void WS2812_Init(void);
void WS2812_SetColor(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Refresh(void);


#endif /* INC_NEOPIXEL_H_ */
