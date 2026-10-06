#include "platform_motor_hardware.h"

#include <stddef.h>

#include "adc.h"
#include "i2c.h"
#include "tim.h"

static const MotorHardwareBinding motor2_hardware =
{
    .pwm_timer = &htim2,
    /* M2_PWM_A: PA3 / TIM2_CH4 -> U phase. */
    .pwm_channel_u = TIM_CHANNEL_4,
    /* M2_PWM_B: PA2 / TIM2_CH3 -> V phase. */
    .pwm_channel_v = TIM_CHANNEL_3,
    /* M2_PWM_C: PA1 / TIM2_CH2 -> W phase. */
    .pwm_channel_w = TIM_CHANNEL_2,
    .adc_trigger_channel = TIM_CHANNEL_1,

    /* ADC2_IN11_M2_A: PC1 -> U-phase current. */
    .phase_u_adc = &hadc2,
    .phase_u_adc_channel = ADC_CHANNEL_11,
    .phase_u_adc_rank = ADC_INJECTED_RANK_1,
    /* ADC3_IN10_M2_B: PC0 -> V-phase current. */
    .phase_v_adc = &hadc3,
    .phase_v_adc_channel = ADC_CHANNEL_10,
    .phase_v_adc_rank = ADC_INJECTED_RANK_1,

    /* M2 encoder: PB10/PB11 -> I2C2 SCL/SDA. */
    .encoder_i2c = &hi2c2,
    .encoder_scl_port = M2_Encoder_SCL_GPIO_Port,
    .encoder_scl_pin = M2_Encoder_SCL_Pin,
    .encoder_sda_port = M2_Encoder_SDA_GPIO_Port,
    .encoder_sda_pin = M2_Encoder_SDA_Pin
};

const MotorHardwareBinding *PlatformMotorHardware_GetBinding(
    MotorId motor_id)
{
    switch (motor_id)
    {
        case MOTOR_ID_1:
            /* Motor 1 hardware is intentionally not bound yet. */
            return NULL;

        case MOTOR_ID_2:
            return &motor2_hardware;

        default:
            return NULL;
    }
}
