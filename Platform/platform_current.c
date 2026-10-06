#include "platform_current.h"

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    const MotorHardwareBinding *hardware;
    volatile uint16_t phase_u_raw;
    volatile uint16_t phase_v_raw;
    volatile uint32_t sample_count;
    bool initialized;
} PlatformCurrentContext;

static PlatformCurrentContext current_context;

static bool PlatformCurrent_IsValidInjectedRank(uint32_t rank)
{
    return (rank == ADC_INJECTED_RANK_1) ||
           (rank == ADC_INJECTED_RANK_2) ||
           (rank == ADC_INJECTED_RANK_3) ||
           (rank == ADC_INJECTED_RANK_4);
}

bool PlatformCurrent_Init(const MotorHardwareBinding *hardware)
{
    ADC_HandleTypeDef *phase_u_adc;
    ADC_HandleTypeDef *phase_v_adc;

    if ((hardware == NULL) ||
        (hardware->phase_u_adc == NULL) ||
        (hardware->phase_v_adc == NULL) ||
        (hardware->phase_u_adc->Instance == NULL) ||
        (hardware->phase_v_adc->Instance == NULL) ||
        (hardware->phase_u_adc == hardware->phase_v_adc) ||
        !PlatformCurrent_IsValidInjectedRank(hardware->phase_u_adc_rank) ||
        !PlatformCurrent_IsValidInjectedRank(hardware->phase_v_adc_rank))
    {
        return false;
    }

    phase_u_adc = hardware->phase_u_adc;
    phase_v_adc = hardware->phase_v_adc;

    current_context.hardware = NULL;
    current_context.phase_u_raw = 0U;
    current_context.phase_v_raw = 0U;
    current_context.sample_count = 0U;
    current_context.initialized = false;

    /* ADC3 is the secondary sampler and must not generate a second ISR. */
    __HAL_ADC_DISABLE_IT(phase_v_adc, ADC_IT_JEOC);
    __HAL_ADC_CLEAR_FLAG(phase_v_adc,
                         ADC_FLAG_JSTRT | ADC_FLAG_JEOC | ADC_FLAG_OVR);

    /* ADC2 is the master sampler whose JEOC interrupt drives the fast task. */
    __HAL_ADC_DISABLE_IT(phase_u_adc, ADC_IT_JEOC);
    __HAL_ADC_CLEAR_FLAG(phase_u_adc,
                         ADC_FLAG_JSTRT | ADC_FLAG_JEOC | ADC_FLAG_OVR);

    if (HAL_ADCEx_InjectedStart(phase_v_adc) != HAL_OK)
    {
        return false;
    }

    if (HAL_ADCEx_InjectedStart_IT(phase_u_adc) != HAL_OK)
    {
        (void)HAL_ADCEx_InjectedStop(phase_v_adc);
        return false;
    }

    current_context.hardware = hardware;
    current_context.initialized = true;

    return true;
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    const MotorHardwareBinding *hardware;

    if (!current_context.initialized)
    {
        return;
    }

    hardware = current_context.hardware;
    if ((hardware == NULL) || (hadc != hardware->phase_u_adc))
    {
        return;
    }

    current_context.phase_u_raw = (uint16_t)HAL_ADCEx_InjectedGetValue(
        hardware->phase_u_adc,
        hardware->phase_u_adc_rank);
    current_context.phase_v_raw = (uint16_t)HAL_ADCEx_InjectedGetValue(
        hardware->phase_v_adc,
        hardware->phase_v_adc_rank);
    current_context.sample_count++;
}
