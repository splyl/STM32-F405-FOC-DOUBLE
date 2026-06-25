#ifndef FOC_PID_H
#define FOC_PID_H

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float last_error;
    float output_min;
    float output_max;
} FOC_PID_t;

void FOC_PID_Init(FOC_PID_t *pid, float kp, float ki, float kd, float output_min, float output_max);
void FOC_PID_SetParams(FOC_PID_t *pid, float kp, float ki, float kd);
void FOC_PID_SetLimit(FOC_PID_t *pid, float output_min, float output_max);
void FOC_PID_Reset(FOC_PID_t *pid);
float FOC_PID_Calculate(FOC_PID_t *pid, float target, float feedback, float dt);

#endif
