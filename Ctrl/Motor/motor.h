#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "stm32f405xx.h"
#include "common_inc.h"
#include "foc_clark_park.h"
#include "FOC_PID.h"

typedef enum {
    MOTOR_RUNNING_VF,
    MOTOR_RUNNING_ENCODER,
    MOTOR_RUNNING_CURRENT_LOOP,
} Motor_Mode;



typedef struct Motor {
    float elc_angle; // 电角度
    float shift_angle; // 机械角度
    float step; // 每次控制周期增加的机械角度
    float encoder_offset; // 编码器电角度零偏，单位 rad
    float encoder_zero_angle; // 编码器机械零点，单位 rad
    float encoder_last_angle; // 上一次编码器机械角度，单位 rad
    uint8_t encoder_zero_valid; // 编码器零点是否已经记录
    float command_angle; // 速度闭环输出的命令机械角度，单位 rad
    float actual_velocity; // 编码器计算出的机械角速度，单位 rad/s
    float velocity_pid_output; // 速度 PID 输出的机械角速度修正量，单位 rad/s
    float velocity_correction_limit; // 速度 PID 输出限幅，单位 rad/s
    FOC_PID_t velocity_pid;
    float voltage_limit; // 电压限制
    float power;
    float open_loop_times; // 开环控制周期，单位 ms
    float target_velocity; // 开环目标机械角速度，单位 rad/s
    uint32_t pole_pair; // 极对数
    uint32_t driver;

    uint32_t TIMER_PERIOD; // 定时器周期

    Motor_Mode mode; // 电机模式

    abc_t Uabc;
    qd_t Uqd;
    alphabeta_t Ualpha_beta;


} Motor;

void FOC_Motor_Init(Motor *motor);
void FOC_Motor_Control(Motor *motor);
void Motor_VF_Control(Motor *motor, float target_velocity);
void Motor_Encoder_Control(Motor *motor);
void Motor_CurrentLoop_Control(Motor *motor);
void Motor_ResetEncoderZero(Motor *motor);
void Motor_Init(Motor *motor, uint32_t pole_pair, float power);
void Motor_MoveOpenLoop(Motor *motor, float target_velocity);



#endif
