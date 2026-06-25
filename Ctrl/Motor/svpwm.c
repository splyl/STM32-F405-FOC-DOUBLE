
#include "svpwm.h"
#include "motor.h"
#include "constant.h"
#include <stdint.h>



uint8_t SectorJudegment(alphabeta_t input)
{
    float A;
    float B;
    float C;
    A = input.beta;
    B =  SQRT_3 *input.alpha - input.beta;
    C = -SQRT_3 *input.alpha - input.beta;
    uint8_t sector = 0;
    uint8_t A_pos = A > 0 ? 1 : 0;
    uint8_t B_pos = B > 0 ? 1 : 0;
    uint8_t C_pos = C > 0 ? 1 : 0;
    uint8_t N = A_pos + B_pos * 2 + C_pos * 4;
    switch (N)
    {
        case 3:
            sector = 1;
            break;
        case 1:
            sector = 2;
            break; 
        case 5:
            sector = 3;
            break;
        case 4:
            sector = 4;
            break;
        case 6:
            sector = 5;
            break;
        case 2:
            sector = 6;
            break;           
    }
    return sector;

}


void SVPWM_Calculate(Motor *motor,uint8_t sector)
{
    float tmp  = motor->TIMER_PERIOD *SQRT_3/ motor->power;

    float X = tmp * motor->Ualpha_beta.beta;
    float Y = tmp * (SQRT_3_DIV_2 * motor->Ualpha_beta.alpha + 0.5f * motor->Ualpha_beta.beta);
    float Z = tmp * (-SQRT_3_DIV_2 * motor->Ualpha_beta.alpha + 0.5f * motor->Ualpha_beta.beta);
    abc_t Tabc_0;
    abc_t Tabc_1;

    float T0 = 0, T1 = 0, T2 = 0;
    switch (sector)
    {
        case 1:
            T1 = -Z;
            T2 = X;
            break;
        case 2:
            T1 = Z;
            T2 = Y;
            break; 
        case 3:
            T1 = X;
            T2 = -Y;
            break;
        case 4:
            T1 = -X;
            T2 = Z;
            break;
        case 5:
            T1 = -Y;
            T2 = -Z;
            break;
        case 6:
            T1 = Y;
            T2 = -X;
            break;           
    }
    if (T1 + T2 > motor->TIMER_PERIOD)
    {
        float sum = T1 + T2;
        T1 = T1 / sum * motor->TIMER_PERIOD;
        T2 = T2 / sum * motor->TIMER_PERIOD;
    }
    T0 = motor->TIMER_PERIOD - T1 - T2;
    Tabc_0.a = T0/4.0f;
    Tabc_0.b = Tabc_0.a + T1/2.0f;
    Tabc_0.c = Tabc_0.b + T2/2.0f;
    switch (sector){
        case 1:                 
            Tabc_1.a = Tabc_0.a;
            Tabc_1.b = Tabc_0.b;
            Tabc_1.c = Tabc_0.c;
            break;
        case 2:                 
            Tabc_1.a = Tabc_0.b;
            Tabc_1.b = Tabc_0.a;
            Tabc_1.c = Tabc_0.c;
            break;
        case 3:                 
            Tabc_1.a = Tabc_0.c;
            Tabc_1.b = Tabc_0.a;
            Tabc_1.c = Tabc_0.b;
            break;
        case 4:                 
            Tabc_1.a = Tabc_0.c;
            Tabc_1.b = Tabc_0.b;
            Tabc_1.c = Tabc_0.a;
            break;
        case 5:                 
            Tabc_1.a = Tabc_0.b;
            Tabc_1.b = Tabc_0.c;
            Tabc_1.c = Tabc_0.a;

            break;
        case 6:                 
            Tabc_1.a = Tabc_0.a;
            Tabc_1.b = Tabc_0.c;
            Tabc_1.c = Tabc_0.b;
            break;
    }
    motor->Uabc.a = Tabc_1.a/motor->TIMER_PERIOD * motor->power;
    motor->Uabc.b = Tabc_1.b/motor->TIMER_PERIOD * motor->power;
    motor->Uabc.c = Tabc_1.c/motor->TIMER_PERIOD * motor->power;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (uint32_t)(Tabc_1.a));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (uint32_t)(Tabc_1.b));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (uint32_t)(Tabc_1.c));
}



void FOC_SVPWM(Motor *motor,float elc_angle)
{
    alphabeta_t input;
    motor->elc_angle = elc_angle;
    
    input = Inverse_Park_transform(motor->Uqd, motor->elc_angle);
    motor->Ualpha_beta = input;

    uint8_t sector = SectorJudegment(input);

    SVPWM_Calculate(motor, sector);
}
