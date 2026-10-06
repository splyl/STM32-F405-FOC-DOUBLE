#include "platform_exti.h"

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "motor_control.h"
#include "platform_led.h"
#include "stm32f4xx_hal.h"

#define KEY_DEBOUNCE_TIME_MS    30U

static uint32_t key_last_interrupt_tick;
static bool key_interrupt_received;

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    uint32_t current_tick;

    if (gpio_pin != Key_Pin_Pin)
    {
        return;
    }

    current_tick = HAL_GetTick();

    if (!key_interrupt_received)
    {
        key_interrupt_received = true;
        key_last_interrupt_tick = current_tick;
        if (MotorControl_ToggleStartStop(MOTOR_ID_2))
        {
            PlatformLed_Toggle();
        }

        return;
    }

    if ((current_tick - key_last_interrupt_tick) < KEY_DEBOUNCE_TIME_MS)
    {
        return;
    }

    key_last_interrupt_tick = current_tick;
    if (MotorControl_ToggleStartStop(MOTOR_ID_2))
    {
        PlatformLed_Toggle();
    }
}
