/*
 * tof_manager.c
 *
 *  Created on: Mar 18, 2026
 *      Author: agheles
 */


/*
 * tof_manager.c
 *
 * Created on: Mar 3, 2026
 * Author: jaja
 */

#include "tof_manager.h"
#include <stdio.h>
#include <string.h>

// Instance globale du tableau de capteurs
TOF_Sensor_t tof_sensors[TOF_COUNT];

// Pointeurs locaux pour les périphériques
static I2C_HandleTypeDef *pI2c;
static UART_HandleTypeDef *pUart;

/**
 * Fonction d'aide interne pour initialiser un seul capteur
 */
static VL53L0X_Error TOF_Init_Single(uint8_t index, uint8_t new_addr, GPIO_TypeDef* XSHUT_Port, uint16_t XSHUT_Pin)
{
    uint8_t VhvSettings, PhaseCal, isApertureSpads;
    uint32_t refSpadCount;
    VL53L0X_Error status;

    // Allumer le capteur
    HAL_GPIO_WritePin(XSHUT_Port, XSHUT_Pin, GPIO_PIN_SET);
    HAL_Delay(50); // Attente boot

    // Configuration initiale (adresse par défaut)
    tof_sensors[index].dev.I2cDevAddr = 0x52;
    tof_sensors[index].dev.I2cHandle = pI2c;

    status = VL53L0X_DataInit(&tof_sensors[index].dev);
    if (status != VL53L0X_ERROR_NONE) return status;

    // Changement d'adresse
    status = VL53L0X_SetDeviceAddress(&tof_sensors[index].dev, new_addr);
    if (status != VL53L0X_ERROR_NONE) return status;
    tof_sensors[index].dev.I2cDevAddr = new_addr;

    // Calibration standard
    status = VL53L0X_StaticInit(&tof_sensors[index].dev);
    if (status != VL53L0X_ERROR_NONE) return status;

    VL53L0X_PerformRefCalibration(&tof_sensors[index].dev, &VhvSettings, &PhaseCal);
    VL53L0X_PerformRefSpadManagement(&tof_sensors[index].dev, &refSpadCount, &isApertureSpads);
    VL53L0X_SetDeviceMode(&tof_sensors[index].dev, VL53L0X_DEVICEMODE_SINGLE_RANGING);

    // Application de la calibration spécifique (Utilisation des variables globales)
    VL53L0X_SetOffsetCalibrationDataMicroMeter(&tof_sensors[index].dev, tof_sensors[index].offset_um);
    VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&tof_sensors[index].dev, TOF_TIMING_BUDGET_US);

    return VL53L0X_ERROR_NONE;
}

/**
 * Initialise tous les capteurs avec leur offset
 */
void TOF_Init_All(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart)
{
    pI2c = hi2c;
    pUart = huart;

    // 1. Attribution des Offsets via les #define du .h
    tof_sensors[0].offset_um = CALIBRATION_OFFSET_TOF1; // T1
    tof_sensors[1].offset_um = CALIBRATION_OFFSET_TOF2; // T2
    tof_sensors[2].offset_um = CALIBRATION_OFFSET_TOF3; // T3

    // 2. Éteindre tous les capteurs
    HAL_GPIO_WritePin(TOF1_XSHUT_GPIO_Port, TOF1_XSHUT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TOF2_XSHUT_GPIO_Port, TOF2_XSHUT_Pin, GPIO_PIN_RESET);
    osDelay(100);

    HAL_UART_Transmit(pUart, (uint8_t*)"Start TOF Init...\r\n", 19, 100);

    // 3. Initialisation séquentielle
    if (TOF_Init_Single(0, TOF1_ADDR_NEW, TOF1_XSHUT_GPIO_Port, TOF1_XSHUT_Pin) == VL53L0X_ERROR_NONE) {
        HAL_UART_Transmit(pUart, (uint8_t*)"T1 Init OK\r\n", 12, 100);
    } else {
        HAL_UART_Transmit(pUart, (uint8_t*)"T1 Init FAIL -> STOP\r\n", 22, 100);
    }

   if (TOF_Init_Single(1, TOF2_ADDR_NEW, TOF2_XSHUT_GPIO_Port, TOF2_XSHUT_Pin) == VL53L0X_ERROR_NONE) {
        HAL_UART_Transmit(pUart, (uint8_t*)"T2 Init OK\r\n", 12, 100);
    } else {
        HAL_UART_Transmit(pUart, (uint8_t*)"T2 Init FAIL\r\n", 14, 100);
    }

}

/**
 * Lit la distance pour les 3 capteurs et met à jour la structure
 */
void TOF_Read_All(void)
{
    VL53L0X_RangingMeasurementData_t rangingData;

    for (int i = 0; i < TOF_COUNT; i++) {
        memset(&rangingData, 0, sizeof(VL53L0X_RangingMeasurementData_t));

        if (VL53L0X_PerformSingleRangingMeasurement(&tof_sensors[i].dev, &rangingData) == VL53L0X_ERROR_NONE) {
            tof_sensors[i].distance_mm = rangingData.RangeMilliMeter;
            tof_sensors[i].status = rangingData.RangeStatus;
        } else {
            tof_sensors[i].distance_mm = 8888;
            tof_sensors[i].status = 255;
        }
    }
}

/**
 * Affiche l'état des capteurs sur le port série
 */
void TOF_Print_Distances(UART_HandleTypeDef *huart)
{
    char buffer[128];
    int len = sprintf(buffer, "T1:%4dmm(st%d) | T2:%4dmm(st%d)\r\n" ,
                      tof_sensors[0].distance_mm, tof_sensors[0].status,
                      tof_sensors[1].distance_mm, tof_sensors[1].status);

    HAL_UART_Transmit(huart, (uint8_t*)buffer, len, 100);
}
