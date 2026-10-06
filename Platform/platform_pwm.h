#ifndef PLATFORM_PWM_H
#define PLATFORM_PWM_H

#include <stdbool.h>

#include "platform_motor_hardware.h"

bool PlatformPwm_Init(const MotorHardwareBinding *hardware);

#endif /* PLATFORM_PWM_H */
