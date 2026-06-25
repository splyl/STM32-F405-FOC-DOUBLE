#include "FOC_PID.h"

static float FOC_PID_Clamp(float value, float min, float max)
{
    if (value > max) {
        return max;
    }

    if (value < min) {
        return min;
    }

    return value;
}

void FOC_PID_Init(FOC_PID_t *pid, float kp, float ki, float kd, float output_min, float output_max)
{
    if (pid == 0) {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->output_min = output_min;
    pid->output_max = output_max;
}

void FOC_PID_SetParams(FOC_PID_t *pid, float kp, float ki, float kd)
{
    if (pid == 0) {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

void FOC_PID_SetLimit(FOC_PID_t *pid, float output_min, float output_max)
{
    if (pid == 0) {
        return;
    }

    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->integral = FOC_PID_Clamp(pid->integral, output_min, output_max);
}

void FOC_PID_Reset(FOC_PID_t *pid)
{
    if (pid == 0) {
        return;
    }

    pid->integral = 0.0f;
    pid->last_error = 0.0f;
}

float FOC_PID_Calculate(FOC_PID_t *pid, float target, float feedback, float dt)
{
    float error;
    float derivative;
    float output;

    if (pid == 0) {
        return 0.0f;
    }

    if (dt <= 0.0f) {
        dt = 0.000001f;
    }

    error = target - feedback;
    pid->integral += error * dt * pid->ki;
    pid->integral = FOC_PID_Clamp(pid->integral, pid->output_min, pid->output_max);

    derivative = (error - pid->last_error) / dt;
    pid->last_error = error;

    output = pid->kp * error + pid->integral + pid->kd * derivative;
    return FOC_PID_Clamp(output, pid->output_min, pid->output_max);
}
