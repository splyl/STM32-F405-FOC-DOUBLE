#include "platform_timebase.h"

#include "motor_control.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

bool PlatformTimebase_Start(void)
{
    return HAL_TIM_Base_Start_IT(&htim4) == HAL_OK;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        MotorControl_MediumFrequencyTask();
    }
}
