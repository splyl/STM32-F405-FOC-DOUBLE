#include "main.h"
#include "common_inc.h"
#include "stm32f405xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "vofa_debug.h"
#include "as5600.h"
#include <stdio.h>

Motor motor1;
uint16_t as5600_angle;
volatile uint16_t adc2_in11_m2_a_raw;
volatile uint16_t adc3_in10_m2_b_raw;
volatile uint8_t adc_m2_injected_updated;
static volatile uint8_t as5600_read_request;
static volatile uint8_t vofa_send_request;




#define TIM4_AS5600_DIV     1U




#define TIM4_VOFA_DIV       25U

void Main(void)
{
    uint8_t as5600_status;

    HAL_StatusTypeDef status;
    HAL_Delay(100);
    motor1.open_loop_times = 0.04f;// TIM2 update period: about 0.04ms
    motor1.target_velocity = 6.28f;
    motor1.voltage_limit = 1.0f+0.3f*motor1.target_velocity;
    motor1.shift_angle = 0.0f;
    motor1.driver=0;
    motor1.TIMER_PERIOD=1680;

    Motor_Init(&motor1, 7, 12);
    motor1.mode = MOTOR_RUNNING_ENCODER;
    motor1.encoder_offset = 0.0f;
    motor1.velocity_correction_limit = 20.0f;
    FOC_PID_SetParams(&motor1.velocity_pid, 0.2f, 0.0f, 0.0f);
    HAL_Delay(100);
    status = AS5600_read_status(&as5600_status);
    (void)status;
    (void)as5600_status;
    if (status != HAL_OK) {
        // 处理错误
        static const char msg[] = "AS5600 read status error\r\n";

        if (HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_TX) {
            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)msg, sizeof(msg) - 1);
        }
    }
    else {
        static char msg[40];
        int len = snprintf(msg, sizeof(msg), "AS5600 status=0x%02X\r\n", as5600_status);

        if (HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_TX) {
            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)msg, len);
        }
    }

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
    HAL_ADCEx_InjectedStart_IT(&hadc2);
    HAL_ADCEx_InjectedStart_IT(&hadc3);
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim4);
    
    while (1) {
        AS5600_Service();

        if ((as5600_read_request != 0U) && (AS5600_IsBusy() == 0U)) {
            as5600_read_request = 0U;
            (void)AS5600_StartReadAngleDMA();
        }

        (void)AS5600_TakeAngle(&as5600_angle);

        if (vofa_send_request != 0U) {
            vofa_send_request = 0U;
            vofa_debug();
        }
    }

    return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM4)
    {
        static uint16_t tim4_count = 0U;

        tim4_count++;
        if ((tim4_count % TIM4_AS5600_DIV) == 0U) {
            as5600_read_request = 1U;
        }

        if ((tim4_count % TIM4_VOFA_DIV) == 0U) {
            vofa_send_request = 1U;
        }
    }
    else if(htim->Instance==TIM2)
    {
        FOC_Motor_Control(&motor1);
    }
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC2) {
        adc2_in11_m2_a_raw = (uint16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
        adc_m2_injected_updated = 1U;
    } else if (hadc->Instance == ADC3) {
        adc3_in10_m2_b_raw = (uint16_t)HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
        adc_m2_injected_updated = 1U;
    }
}
