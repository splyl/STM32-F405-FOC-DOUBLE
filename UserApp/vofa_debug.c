#include "foc_clark_park.h"
#include "vofa_debug.h"
#include "dma.h"
#include "motor.h"
#include "tim.h"
#include "usart.h"
#include "constant.h"
VOFA_SEND_DATA VOFA_DATA = {.tail = {0x00, 0x00, 0x80, 0x7f}};

extern abc_t Iabc; 
extern qd_t Iqd;
extern alphabeta_t Ialphabeta;
extern Motor motor1;
extern uint16_t as5600_angle;
extern volatile uint16_t adc2_in11_m2_a_raw;
extern volatile uint16_t adc3_in10_m2_b_raw;
void vofa_debug(void)
{
    if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX)
        return;  // 上一次 DMA 传输未完成，跳过本帧

    // 清零所有通道，避免发送垃圾数据
    for (int i = 0; i < VOFA_COUNT; i++)
        VOFA_DATA.fdata[i] = 0.0f;

    VOFA_DATA.fdata[0] = motor1.Uabc.a;
    VOFA_DATA.fdata[1] = motor1.Uabc.b;
    VOFA_DATA.fdata[2] = motor1.Uabc.c;
    VOFA_DATA.fdata[3] = (float)as5600_angle*2*PI/4096;
    // VOFA_DATA.fdata[4] = motor1.Ualpha_beta.alpha+3;
    // VOFA_DATA.fdata[5] = motor1.Uqd.q+5;
    // VOFA_DATA.fdata[6] = motor1.Uqd.d+5;
    VOFA_DATA.fdata[4] = motor1.elc_angle;
    VOFA_DATA.fdata[5] = (float)adc2_in11_m2_a_raw;
    VOFA_DATA.fdata[6] = (float)adc3_in10_m2_b_raw;
    VOFA_DATA.fdata[7] = (float)adc2_in11_m2_a_raw * 3.3f / 4095.0f;
    VOFA_DATA.fdata[8] = (float)adc3_in10_m2_b_raw * 3.3f / 4095.0f;
    VOFA_DATA.fdata[9] = motor1.actual_velocity;
    VOFA_DATA.fdata[10] = motor1.target_velocity;
    VOFA_DATA.fdata[11] = motor1.Uqd.q;
    VOFA_DATA.fdata[12] = motor1.velocity_pid_output;
    VOFA_DATA.fdata[13] = motor1.command_angle;

    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&VOFA_DATA, sizeof(VOFA_DATA));
}
