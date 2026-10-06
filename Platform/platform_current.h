#ifndef PLATFORM_CURRENT_H
#define PLATFORM_CURRENT_H

#include <stdbool.h>

#include "platform_motor_hardware.h"

bool PlatformCurrent_Init(const MotorHardwareBinding *hardware);

#endif /* PLATFORM_CURRENT_H */
