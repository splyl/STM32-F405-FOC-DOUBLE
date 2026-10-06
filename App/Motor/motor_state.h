#ifndef MOTOR_STATE_H
#define MOTOR_STATE_H

#include "motor_types.h"

void MotorState_Init(MotorControl *motor, MotorId motor_id);
void MotorState_Process(MotorControl *motor, MotorCommand command);
MotorState MotorState_Get(const MotorControl *motor);

#endif /* MOTOR_STATE_H */
