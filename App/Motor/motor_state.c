#include "motor_state.h"

#include <stddef.h>

void MotorState_Init(MotorControl *motor, MotorId motor_id)
{
    if (motor == NULL)
    {
        return;
    }

    motor->id = motor_id;
    motor->state = MOTOR_STATE_BOOT;
    motor->pending_command = MOTOR_COMMAND_NONE;
    motor->hardware = NULL;
}

void MotorState_Process(MotorControl *motor, MotorCommand command)
{
    if (motor == NULL)
    {
        return;
    }

    switch (motor->state)
    {
        case MOTOR_STATE_BOOT:
            motor->state = MOTOR_STATE_IDLE;
            break;

        case MOTOR_STATE_IDLE:
            if (command == MOTOR_COMMAND_START)
            {
                motor->state = MOTOR_STATE_START_REQUEST;
            }
            break;

        case MOTOR_STATE_START_REQUEST:
            motor->state = (command == MOTOR_COMMAND_STOP) ?
                           MOTOR_STATE_STOP_REQUEST :
                           MOTOR_STATE_STARTING;
            break;

        case MOTOR_STATE_STARTING:
            motor->state = (command == MOTOR_COMMAND_STOP) ?
                           MOTOR_STATE_STOP_REQUEST :
                           MOTOR_STATE_RUN;
            break;

        case MOTOR_STATE_RUN:
            if (command == MOTOR_COMMAND_STOP)
            {
                motor->state = MOTOR_STATE_STOP_REQUEST;
            }
            break;

        case MOTOR_STATE_STOP_REQUEST:
            motor->state = MOTOR_STATE_STOPPING;
            break;

        case MOTOR_STATE_STOPPING:
            motor->state = MOTOR_STATE_IDLE;
            break;

        case MOTOR_STATE_DISABLED:
        case MOTOR_STATE_FAULT:
            break;

        default:
            motor->state = MOTOR_STATE_FAULT;
            break;
    }
}

MotorState MotorState_Get(const MotorControl *motor)
{
    if (motor == NULL)
    {
        return MOTOR_STATE_FAULT;
    }

    return motor->state;
}
