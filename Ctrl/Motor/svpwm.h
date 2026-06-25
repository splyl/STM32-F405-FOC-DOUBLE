#ifndef SVPWM_H
#define SVPWM_H
#include <stdint.h>
#include "foc_clark_park.h"
#include "constant.h"

typedef struct Motor Motor;

uint8_t SectorJudegment(alphabeta_t input);
void SVPWM_Calculate(Motor *motor, uint8_t sector);
void FOC_SVPWM(Motor *motor, float elc_angle);

#endif
