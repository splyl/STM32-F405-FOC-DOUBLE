#include "platform_led.h"

#include "main.h"
#include "stm32f4xx_hal.h"

void PlatformLed_Toggle(void)
{
    HAL_GPIO_TogglePin(Led_Pin_GPIO_Port, Led_Pin_Pin);
}
