/*
 * app_config.h
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */

#ifndef INC_BATTERY_CONFIG_H_
#define INC_BATTERY_CONFIG_H_


// --- Paramètres Batterie (Inchangé) ---
#define R_HIGH_OHM              68000.0f
#define R_LOW_OHM               10000.0f
#define ADC_REF_VOLTAGE         3.3f
#define ADC_RESOLUTION          4095.0f
#define BATTERY_CONVERSION_COEFF ((ADC_REF_VOLTAGE / ADC_RESOLUTION) * ((R_HIGH_OHM + R_LOW_OHM) / R_LOW_OHM))

#define BAT_LEVEL_FULL          16.2f
#define BAT_LEVEL_MID           15.0f
#define BAT_LEVEL_LOW           14.5f


#endif /* INC_BATTERY_CONFIG_H_ */
