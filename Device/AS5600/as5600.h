#ifndef DEVICE_AS5600_H
#define DEVICE_AS5600_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

#define AS5600_I2C_ADDRESS_7BIT      0x36U

#define AS5600_REG_STATUS            0x0BU
#define AS5600_REG_RAW_ANGLE_H       0x0CU
#define AS5600_REG_RAW_ANGLE_L       0x0DU
#define AS5600_REG_ANGLE_H           0x0EU
#define AS5600_REG_ANGLE_L           0x0FU
#define AS5600_REG_AGC               0x1AU
#define AS5600_REG_MAGNITUDE_H       0x1BU
#define AS5600_REG_MAGNITUDE_L       0x1CU

bool AS5600_Init(void);
HAL_StatusTypeDef AS5600_ReadAngle(uint16_t *angle_12bit);
HAL_StatusTypeDef AS5600_ReadRawAngle(uint16_t *angle_12bit);
HAL_StatusTypeDef AS5600_ReadStatus(uint8_t *status);
HAL_StatusTypeDef AS5600_ReadAgc(uint8_t *agc);
HAL_StatusTypeDef AS5600_ReadMagnitude(uint16_t *magnitude_12bit);
HAL_StatusTypeDef AS5600_StartReadAngleDma(void);
void AS5600_Service(void);
bool AS5600_TakeAngle(uint16_t *angle_12bit);
bool AS5600_IsAngleUpdated(void);
bool AS5600_IsBusy(void);
uint16_t AS5600_GetAngle(void);
HAL_StatusTypeDef AS5600_GetLastStatus(void);

#endif /* DEVICE_AS5600_H */
