/*
 * tof_manager.h
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */

#ifndef INC_TOF_MANAGER_H_
#define INC_TOF_MANAGER_H_

#include "main.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"



#define TOF_COUNT 2

// Adresses I2C (format 8-bit : adresse 7-bit << 1)
#define TOF1_ADDR_NEW 0x54
#define TOF2_ADDR_NEW 0x56
#define TOF3_ADDR_NEW 0x58

// ============================================================================
// --- RÉGLAGES ET CALIBRATION (À MODIFIER ICI) ---
// ============================================================================

/* * TIMING BUDGET : Temps accordé au capteur pour faire sa mesure.
 * Plus c'est haut, plus c'est précis et stable, mais plus c'est lent.
 * Défaut: 33000 (33ms). Recommandé: 50000 (50ms). Max Haute Précision: 200000 (200ms).
 */
#define TOF_TIMING_BUDGET_US  50000

/* * OFFSETS : Calibration des distances en micromètres (1 mm = 1000 µm).
 * Formule : (Distance_Réelle_Mesurée_Au_Mètre_Ruban - Distance_Affichée_Console) * 1000
 * Exemple : S'il affiche 94mm alors que l'objet est à 55mm -> (55 - 94) * 1000 = -39000
 */
#define CALIBRATION_OFFSET_TOF1  41000  // Offset pour le capteur Avant Gauche
#define CALIBRATION_OFFSET_TOF2  40000  // Offset pour le capteur Avant Droit
#define CALIBRATION_OFFSET_TOF3  9000  // Offset pour le capteur Arrière


// ============================================================================
// --- STRUCTURES & PROTOTYPES ---
// ============================================================================

typedef struct {
    VL53L0X_Dev_t dev;        // La structure interne de l'API VL53L0X
    uint16_t distance_mm;     // Dernière distance lue
    uint8_t  status;          // Dernier statut lu (0 = OK)
    int32_t  offset_um;       // Offset de calibration en micromètres
} TOF_Sensor_t;

extern TOF_Sensor_t tof_sensors[TOF_COUNT];

void TOF_Init_All(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);
void TOF_Read_All(void);
void TOF_Print_Distances(UART_HandleTypeDef *huart);



#endif /* INC_TOF_MANAGER_H_ */
