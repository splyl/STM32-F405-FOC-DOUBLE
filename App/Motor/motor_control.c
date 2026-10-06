#include "motor_control.h"

#include <stddef.h>

#include "as5600.h"
#include "motor_state.h"
#include "platform_current.h"
#include "platform_encoder_port.h"
#include "platform_motor_hardware.h"
#include "platform_pwm.h"
static MotorControl motors[MOTOR_ID_COUNT];

static bool MotorControl_BindHardware(MotorControl *motor,
                                      MotorId motor_id);

static MotorControl *MotorControl_GetMotor(MotorId motor_id)
{
    MotorControl *motor;

    if (motor_id >= MOTOR_ID_COUNT)
    {
        return NULL;
    }

    motor = &motors[motor_id];
    if ((motor->state == MOTOR_STATE_DISABLED) ||
        (motor->id != motor_id))
    {
        return NULL;
    }

    return motor;
}

static bool MotorControl_SubmitCommand(MotorControl *motor,
                                       MotorCommand command)
{
    if ((motor == NULL) ||
        (motor->pending_command != MOTOR_COMMAND_NONE))
    {
        return false;
    }

    motor->pending_command = command;
    return true;
}

bool MotorControl_Init(MotorId motor_id)
{
    MotorControl *motor;

    if (motor_id >= MOTOR_ID_COUNT)
    {
        return false;
    }

    motor = &motors[motor_id];
    MotorState_Init(motor, motor_id);

    /* TODO: Bind and initialize the hardware and control modules for motor_id. */
    if (!MotorControl_BindHardware(motor, motor_id))
    {
        motor->state = MOTOR_STATE_DISABLED;
        return false;
    }
    
    if (!PlatformPwm_Init(motor->hardware))
    {
        motor->state = MOTOR_STATE_FAULT;
        return false;
    }

    if (!PlatformCurrent_Init(motor->hardware))
    {
        motor->state = MOTOR_STATE_FAULT;
        return false;
    }

    if (!PlatformEncoderPort_Init(motor->hardware))
    {
        motor->state = MOTOR_STATE_FAULT;
        return false;
    }

    if (!AS5600_Init())
    {
        motor->state = MOTOR_STATE_FAULT;
        return false;
    }

    motor->state = MOTOR_STATE_IDLE;
    return true;
}

bool MotorControl_Start(MotorId motor_id)
{
    MotorControl *motor;

    motor = MotorControl_GetMotor(motor_id);
    if ((motor == NULL) || (motor->state != MOTOR_STATE_IDLE))
    {
        return false;
    }

    return MotorControl_SubmitCommand(motor, MOTOR_COMMAND_START);
}

bool MotorControl_Stop(MotorId motor_id)
{
    MotorControl *motor;

    motor = MotorControl_GetMotor(motor_id);
    if ((motor == NULL) ||
        ((motor->state != MOTOR_STATE_START_REQUEST) &&
         (motor->state != MOTOR_STATE_STARTING) &&
         (motor->state != MOTOR_STATE_RUN)))
    {
        return false;
    }

    return MotorControl_SubmitCommand(motor, MOTOR_COMMAND_STOP);
}

bool MotorControl_ToggleStartStop(MotorId motor_id)
{
    MotorControl *motor;

    motor = MotorControl_GetMotor(motor_id);
    if (motor == NULL)
    {
        return false;
    }

    if (motor->state == MOTOR_STATE_IDLE)
    {
        return MotorControl_Start(motor_id);
    }

    return MotorControl_Stop(motor_id);
}

MotorState MotorControl_GetState(MotorId motor_id)
{
    MotorControl *motor;

    motor = MotorControl_GetMotor(motor_id);
    if (motor == NULL)
    {
        return MOTOR_STATE_DISABLED;
    }

    return MotorState_Get(motor);
}

void MotorControl_MediumFrequencyTask(void)
{
    MotorId motor_id;

    for (motor_id = MOTOR_ID_1; motor_id < MOTOR_ID_COUNT; motor_id++)
    {
        MotorControl *motor;
        MotorCommand command;

        motor = MotorControl_GetMotor(motor_id);
        if (motor == NULL)
        {
            continue;
        }

        command = motor->pending_command;
        motor->pending_command = MOTOR_COMMAND_NONE;

        MotorState_Process(motor, command);
    }
}
static bool MotorControl_BindHardware(MotorControl *motor,
                                      MotorId motor_id)
{
    if ((motor == NULL) ||
        (motor_id >= MOTOR_ID_COUNT) ||
        (motor->id != motor_id))
    {
        return false;
    }

    motor->hardware = PlatformMotorHardware_GetBinding(motor_id);
    return motor->hardware != NULL;
}
