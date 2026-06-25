#ifndef __AS5600_H
#define __AS5600_H

#include <stdint.h>
#include "stm32f4xx_hal.h"


#define AS5600_I2C_ADDR_7BIT      0x36//设备地址

// 常用寄存器
#define AS5600_REG_STATUS         0x0B
#define AS5600_REG_RAW_ANGLE_H    0x0C
#define AS5600_REG_RAW_ANGLE_L    0x0D
#define AS5600_REG_ANGLE_H        0x0E
#define AS5600_REG_ANGLE_L        0x0F
#define AS5600_REG_AGC            0x1A
#define AS5600_REG_MAG_H          0x1B
#define AS5600_REG_MAG_L          0x1C

HAL_StatusTypeDef AS5600_read_angle(uint16_t *angle12);
HAL_StatusTypeDef AS5600_read_raw_angle12(uint16_t *raw12);
HAL_StatusTypeDef AS5600_read_status(uint8_t *st);
HAL_StatusTypeDef AS5600_read_agc(uint8_t *agc);
HAL_StatusTypeDef AS5600_read_magnitude12(uint16_t *mag12);
HAL_StatusTypeDef AS5600_StartReadAngleDMA(void);
void AS5600_Service(void);
uint8_t AS5600_TakeAngle(uint16_t *angle12);
uint8_t AS5600_IsAngleUpdated(void);
uint8_t AS5600_IsBusy(void);
uint16_t AS5600_GetAngle(void);
HAL_StatusTypeDef AS5600_GetLastStatus(void);


#endif
