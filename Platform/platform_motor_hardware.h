#ifndef PLATFORM_MOTOR_HARDWARE_H
#define PLATFORM_MOTOR_HARDWARE_H

#include "motor_types.h"
#include "stm32f4xx_hal.h"

typedef struct MotorHardwareBinding
{
    TIM_HandleTypeDef *pwm_timer;
    uint32_t pwm_channel_u;
    uint32_t pwm_channel_v;
    uint32_t pwm_channel_w;
    uint32_t adc_trigger_channel;

    ADC_HandleTypeDef *phase_u_adc;
    uint32_t phase_u_adc_channel;
    uint32_t phase_u_adc_rank;
    ADC_HandleTypeDef *phase_v_adc;
    uint32_t phase_v_adc_channel;
    uint32_t phase_v_adc_rank;

    I2C_HandleTypeDef *encoder_i2c;
    GPIO_TypeDef *encoder_scl_port;
    uint16_t encoder_scl_pin;
    GPIO_TypeDef *encoder_sda_port;
    uint16_t encoder_sda_pin;
} MotorHardwareBinding;

const MotorHardwareBinding *PlatformMotorHardware_GetBinding(
    MotorId motor_id);

#endif /* PLATFORM_MOTOR_HARDWARE_H */
