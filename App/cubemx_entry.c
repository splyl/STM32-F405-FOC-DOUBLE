#include "app_main.h"
#include "common_inc.h"

void Main(void)
{
    App_Init();

    while (1)
    {
        App_BackgroundTask();
    }
}
