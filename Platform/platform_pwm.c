#include "platform_pwm.h"

#include <stddef.h>

static bool PlatformPwm_IsValidChannel(uint32_t channel)
{
    return (channel == TIM_CHANNEL_1) ||
           (channel == TIM_CHANNEL_2) ||
           (channel == TIM_CHANNEL_3) ||
           (channel == TIM_CHANNEL_4);
}

static bool PlatformPwm_AreChannelsValid(
    const MotorHardwareBinding *hardware)
{
    if (!PlatformPwm_IsValidChannel(hardware->pwm_channel_u) ||
        !PlatformPwm_IsValidChannel(hardware->pwm_channel_v) ||
        !PlatformPwm_IsValidChannel(hardware->pwm_channel_w) ||
        !PlatformPwm_IsValidChannel(hardware->adc_trigger_channel))
    {
        return false;
    }

    if ((hardware->pwm_channel_u == hardware->pwm_channel_v) ||
        (hardware->pwm_channel_u == hardware->pwm_channel_w) ||
        (hardware->pwm_channel_v == hardware->pwm_channel_w) ||
        (hardware->adc_trigger_channel == hardware->pwm_channel_u) ||
        (hardware->adc_trigger_channel == hardware->pwm_channel_v) ||
        (hardware->adc_trigger_channel == hardware->pwm_channel_w))
    {
        return false;
    }

    return true;
}

bool PlatformPwm_Init(const MotorHardwareBinding *hardware)
{
    TIM_HandleTypeDef *timer;
    uint32_t inactive_compare;

    if ((hardware == NULL) ||
        (hardware->pwm_timer == NULL) ||
        (hardware->pwm_timer->Instance == NULL) ||
        !PlatformPwm_AreChannelsValid(hardware))
    {
        return false;
    }

    timer = hardware->pwm_timer;

    /* Keep all bridge command outputs disabled during initialization. */
    (void)HAL_TIM_PWM_Stop(timer, hardware->pwm_channel_u);
    (void)HAL_TIM_PWM_Stop(timer, hardware->pwm_channel_v);
    (void)HAL_TIM_PWM_Stop(timer, hardware->pwm_channel_w);

    /*
     * The configured outputs use PWM mode 2.  A compare value greater than
     * ARR keeps OCxREF inactive for the whole count cycle when PWM is later
     * enabled.  The channels remain stopped here; this is only a safe preload.
     */
    inactive_compare = __HAL_TIM_GET_AUTORELOAD(timer) + 1U;
    __HAL_TIM_SET_COMPARE(timer,
                          hardware->pwm_channel_u,
                          inactive_compare);
    __HAL_TIM_SET_COMPARE(timer,
                          hardware->pwm_channel_v,
                          inactive_compare);
    __HAL_TIM_SET_COMPARE(timer,
                          hardware->pwm_channel_w,
                          inactive_compare);

    __HAL_TIM_SET_COUNTER(timer, 0U);
    __HAL_TIM_CLEAR_FLAG(timer,
                         TIM_FLAG_UPDATE |
                         TIM_FLAG_CC1 |
                         TIM_FLAG_CC2 |
                         TIM_FLAG_CC3 |
                         TIM_FLAG_CC4);

    return true;
}
