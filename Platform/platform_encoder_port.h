#ifndef PLATFORM_ENCODER_PORT_H
#define PLATFORM_ENCODER_PORT_H

#include <stdbool.h>
#include <stdint.h>

#include "platform_motor_hardware.h"

bool PlatformEncoderPort_Init(const MotorHardwareBinding *hardware);
bool PlatformEncoderPort_Recover(void);

HAL_StatusTypeDef PlatformEncoderPort_IsDeviceReady(
    uint16_t device_address,
    uint32_t trials,
    uint32_t timeout_ms);
HAL_StatusTypeDef PlatformEncoderPort_MemRead(
    uint16_t device_address,
    uint16_t register_address,
    uint16_t register_address_size,
    uint8_t *data,
    uint16_t length,
    uint32_t timeout_ms);
HAL_StatusTypeDef PlatformEncoderPort_MemReadDma(
    uint16_t device_address,
    uint16_t register_address,
    uint16_t register_address_size,
    uint8_t *data,
    uint16_t length);

void PlatformEncoderPort_RxCompleteCallback(void);
void PlatformEncoderPort_ErrorCallback(void);

#endif /* PLATFORM_ENCODER_PORT_H */
