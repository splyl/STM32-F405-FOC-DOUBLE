#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdbool.h>

#include "motor_types.h"

bool MotorControl_Init(MotorId motor_id);
bool MotorControl_Start(MotorId motor_id);
bool MotorControl_Stop(MotorId motor_id);
bool MotorControl_ToggleStartStop(MotorId motor_id);
MotorState MotorControl_GetState(MotorId motor_id);
void MotorControl_MediumFrequencyTask(void);

#endif /* MOTOR_CONTROL_H */
