#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

// Fonction d'écriture multiple via HAL I2C
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
    // HAL_I2C_Mem_Write gère l'envoi de l'adresse registre (index) + les données
    if (HAL_I2C_Mem_Write(Dev->I2cHandle, Dev->I2cDevAddr, index, I2C_MEMADD_SIZE_8BIT, pdata, count, 1000) != HAL_OK) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    return VL53L0X_ERROR_NONE;
}

// Fonction de lecture multiple via HAL I2C
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
    if (HAL_I2C_Mem_Read(Dev->I2cHandle, Dev->I2cDevAddr, index, I2C_MEMADD_SIZE_8BIT, pdata, count, 1000) != HAL_OK) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
    return VL53L0X_WriteMulti(Dev, index, &data, 1);
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
    uint8_t buffer[2];
    // VL53L0X est Big Endian (MSB first)
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data & 0xFF);
    return VL53L0X_WriteMulti(Dev, index, buffer, 2);
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    uint8_t buffer[4];
    buffer[0] = (uint8_t)(data >> 24);
    buffer[1] = (uint8_t)((data & 0xFF0000) >> 16);
    buffer[2] = (uint8_t)((data & 0xFF00) >> 8);
    buffer[3] = (uint8_t)(data & 0xFF);
    return VL53L0X_WriteMulti(Dev, index, buffer, 4);
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t data;

    Status = VL53L0X_RdByte(Dev, index, &data);
    if (Status != VL53L0X_ERROR_NONE) return Status;

    data = (data & AndData) | OrData;
    return VL53L0X_WrByte(Dev, index, data);
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{
    return VL53L0X_ReadMulti(Dev, index, data, 1);
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t buffer[2];

    Status = VL53L0X_ReadMulti(Dev, index, buffer, 2);
    if (Status == VL53L0X_ERROR_NONE) {
        *data = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
    }
    return Status;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t buffer[4];

    Status = VL53L0X_ReadMulti(Dev, index, buffer, 4);
    if (Status == VL53L0X_ERROR_NONE) {
        *data = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
                ((uint32_t)buffer[2] << 8)  | (uint32_t)buffer[3];
    }
    return Status;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
    // Petit délai pour laisser le temps au capteur (1ms)
    HAL_Delay(1);
    return VL53L0X_ERROR_NONE;
}
