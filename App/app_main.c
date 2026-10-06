#include "app_main.h"

#include "motor_control.h"
#include "platform_timebase.h"

void App_Init(void)
{
    (void)MotorControl_Init(MOTOR_ID_2);
    (void)PlatformTimebase_Start();
}

void App_BackgroundTask(void)
{
}
