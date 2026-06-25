#include "motor.h"
#include "svpwm.h"
#include "constant.h"
#include <math.h>
#include <stdint.h>
#include "as5600.h"
#include "FOC_PID.h"

extern uint16_t as5600_angle;

#define AS5600_COUNT_TO_RAD (2.0f * PI / 4096.0f)
#define MOTOR_TWO_PI (2.0f * PI)

void FOC_Motor_Init(Motor *motor)
{
    motor->mode = MOTOR_RUNNING_VF;
    motor->Uqd.d = 0.0f;
    motor->Uqd.q = motor->voltage_limit;
    motor->step = 0.01f;
    motor->encoder_offset = 0.0f;
    motor->encoder_zero_angle = 0.0f;
    motor->encoder_last_angle = 0.0f;
    motor->encoder_zero_valid = 0U;
    motor->command_angle = 0.0f;
    motor->actual_velocity = 0.0f;
    motor->velocity_pid_output = 0.0f;
    motor->velocity_correction_limit = 20.0f;
    FOC_PID_Init(&motor->velocity_pid,
                 0.2f,
                 0.0f,
                 0.0f,
                 -motor->velocity_correction_limit,
                 motor->velocity_correction_limit);
    motor->shift_angle = 0.0f;
    motor->elc_angle = 0.0f;
    motor->Uabc.a = 0.0f;
    motor->Uabc.b = 0.0f;
    motor->Uabc.c = 0.0f;
}

void Motor_Init(Motor *motor, uint32_t pole_pair, float power)
{
    motor->pole_pair = pole_pair;
    motor->power = power;
    FOC_Motor_Init(motor);
}

static float Motor_WrapAngle(float angle)
{
    while (angle >= MOTOR_TWO_PI) {
        angle -= MOTOR_TWO_PI;
    }

    while (angle < 0.0f) {
        angle += MOTOR_TWO_PI;
    }

    return angle;
}

static float Motor_WrapDelta(float delta)
{
    while (delta > PI) {
        delta -= MOTOR_TWO_PI;
    }

    while (delta < -PI) {
        delta += MOTOR_TWO_PI;
    }

    return delta;
}

void Motor_ResetEncoderZero(Motor *motor)
{
    motor->encoder_zero_angle = 0.0f;
    motor->encoder_last_angle = 0.0f;
    motor->encoder_zero_valid = 0U;
    motor->command_angle = 0.0f;
    motor->actual_velocity = 0.0f;
    motor->velocity_pid_output = 0.0f;
    FOC_PID_Reset(&motor->velocity_pid);
}

//开环下的电角度计算
void Motor_UpdateOpenLoopAngle(Motor *motor, float target_velocity)
{
    motor->target_velocity = target_velocity;
    motor->step = motor->target_velocity * motor->open_loop_times * 0.001f;
    motor->shift_angle = Motor_WrapAngle(motor->shift_angle + motor->step);
    // motor->shift_angle = as5600_angle*2*PI/4096;
    motor->elc_angle = Motor_WrapAngle(motor->shift_angle * (float)motor->pole_pair);
}

// 使用已经累加好的电角度计算 SVPWM
void Motor_VF_Control(Motor *motor,float target_velocity)
{
    Motor_UpdateOpenLoopAngle(motor, target_velocity);

    motor->Uqd.d = 0.0f;
    motor->Uqd.q = motor->voltage_limit;
    FOC_SVPWM(motor, motor->elc_angle);
}



void Motor_Encoder_Control(Motor *motor)
{
    float dt = motor->open_loop_times * 0.001f;
    float raw_angle = Motor_WrapAngle((float)as5600_angle * AS5600_COUNT_TO_RAD);
    float mech_angle;
    float delta;
    float command_velocity;

    if (dt <= 0.0f) {
        dt = 0.00004f;
    }

    if (motor->encoder_zero_valid == 0U) {
        motor->encoder_zero_angle = raw_angle;
        motor->encoder_last_angle = 0.0f;
        motor->command_angle = 0.0f;
        motor->encoder_zero_valid = 1U;
        motor->actual_velocity = 0.0f;
        motor->velocity_pid_output = 0.0f;
        FOC_PID_Reset(&motor->velocity_pid);
    }

    mech_angle = Motor_WrapAngle(raw_angle - motor->encoder_zero_angle);
    delta = Motor_WrapDelta(mech_angle - motor->encoder_last_angle);

    motor->shift_angle = mech_angle;
    motor->actual_velocity = delta / dt;
    motor->encoder_last_angle = mech_angle;

    FOC_PID_SetLimit(&motor->velocity_pid,
                     -motor->velocity_correction_limit,
                     motor->velocity_correction_limit);
    motor->velocity_pid_output = FOC_PID_Calculate(&motor->velocity_pid,
                                                   motor->target_velocity,
                                                   motor->actual_velocity,
                                                   dt);
    command_velocity = motor->target_velocity + motor->velocity_pid_output;
    motor->step = command_velocity * dt;
    motor->command_angle = Motor_WrapAngle(motor->command_angle + motor->step);
    motor->elc_angle = Motor_WrapAngle(motor->command_angle * (float)motor->pole_pair + motor->encoder_offset);

    motor->Uqd.d = 0.0f;
    motor->Uqd.q = motor->voltage_limit;
    FOC_SVPWM(motor, motor->elc_angle);

}
void Motor_CurrentLoop_Control(Motor *motor)
{
    
}

void FOC_Motor_Control(Motor *motor)
{
    switch (motor->mode) {
        case MOTOR_RUNNING_VF:
            Motor_VF_Control(motor, motor->target_velocity);
            break;

        case MOTOR_RUNNING_ENCODER:
            Motor_Encoder_Control(motor);
            break;

        case MOTOR_RUNNING_CURRENT_LOOP:
            Motor_CurrentLoop_Control(motor);
            break;

        default:
            break;
    }
}
