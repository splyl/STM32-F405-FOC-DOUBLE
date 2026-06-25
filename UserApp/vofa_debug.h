#ifndef _VOFA_DEBUG_H_
#define _VOFA_DEBUG_H_

#include "stm32f4xx_hal.h"

#define VOFA_COUNT 20

typedef  struct{
    float fdata[VOFA_COUNT];
    unsigned char tail[4];
}VOFA_SEND_DATA;


void vofa_debug(void);
#endif