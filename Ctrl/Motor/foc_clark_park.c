#include "foc_clark_park.h"
#include "math.h"
#include "constant.h"
#include "fast_trig.h"

static float x = 0.0f;

abc_t Iabc;
qd_t Iqd;
alphabeta_t Ialphabeta;



alphabeta_t Clark_transform(abc_t Iabc)
{
    alphabeta_t result;
    // 这里有作等幅值变化
    result.alpha = Iabc.a;
    result.beta = (Iabc.a + 2.0f * Iabc.b) / SQRT_3;
    return result;
}

abc_t Inverse_Clark_transform(alphabeta_t Ialphabeta)
{
    abc_t result;
    result.a = Ialphabeta.alpha;
    result.b = -Ialphabeta.alpha / 2.0f + SQRT_3 * Ialphabeta.beta / 2.0f;
    result.c = -Ialphabeta.alpha / 2.0f - SQRT_3 * Ialphabeta.beta / 2.0f;
    return result;
}

qd_t Park_transform(alphabeta_t Ialphabeta, float theta)
{
    qd_t result;
    result.d = Ialphabeta.alpha * arm_cos_f32(theta) + Ialphabeta.beta * arm_sin_f32(theta);
    result.q = -Ialphabeta.alpha * arm_sin_f32(theta) + Ialphabeta.beta * arm_cos_f32(theta);
    return result;
}

alphabeta_t Inverse_Park_transform(qd_t Iqd, float theta)
{
    alphabeta_t result;
    result.alpha = Iqd.d * arm_cos_f32(theta) - Iqd.q * arm_sin_f32(theta);
    result.beta = Iqd.d * arm_sin_f32(theta) + Iqd.q * arm_cos_f32(theta);
    return result;
}


void Clark_Park_Transform(void)
{
    // Clarke Transform
    Iabc.a = arm_sin_f32(x);
    Iabc.b = arm_sin_f32(x + 2.0f * PI_3);
    Iabc.c = arm_sin_f32(x + 2.0f * PI_3 + 2.0f * PI_3);
    x += 0.001f;
    Ialphabeta = Clark_transform(Iabc);
    Iqd = Park_transform(Ialphabeta, atan2f(Ialphabeta.beta, Ialphabeta.alpha));
}
