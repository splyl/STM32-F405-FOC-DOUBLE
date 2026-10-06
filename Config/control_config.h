#ifndef CONTROL_CONFIG_H
#define CONTROL_CONFIG_H

#include "board_config.h"
#include "motor_config.h"

//速度环以及电流环频率
#define CONTROL_CURRENT_LOOP_FREQUENCY_HZ \
    BOARD_ADC_TRIGGER_FREQUENCY_HZ 
#define CONTROL_SPEED_LOOP_FREQUENCY_HZ \
    BOARD_SPEED_TIMER_FREQUENCY_HZ
//速度环以及电流环时间
#define CONTROL_CURRENT_LOOP_PERIOD_S \
    (1.0f / (float)CONTROL_CURRENT_LOOP_FREQUENCY_HZ)
#define CONTROL_SPEED_LOOP_PERIOD_S \
    (1.0f / (float)CONTROL_SPEED_LOOP_FREQUENCY_HZ)

// #define CONTROL_SPEED_LOOP_DIVIDER \
//     (CONTROL_CURRENT_LOOP_FREQUENCY_HZ / CONTROL_SPEED_LOOP_FREQUENCY_HZ)

/* Safe initial references ------------------------------------------------ */
// #define CONTROL_DEFAULT_ID_REFERENCE_A             0.0f
// #define CONTROL_DEFAULT_IQ_REFERENCE_A             0.0f
// #define CONTROL_DEFAULT_PWM_DUTY \
//     BOARD_PWM_NEUTRAL_DUTY

// /* Output limits currently known ------------------------------------------ */
// #define CONTROL_MAX_PHASE_CURRENT_A \
//     MOTOR1_MAX_PHASE_CURRENT_A
// #define CONTROL_MAX_MECHANICAL_SPEED_RAD_PER_SEC \
//     MOTOR1_MAX_MECHANICAL_SPEED_RAD_PER_SEC

/* Controller gains and ramps still require identification/tuning. -------- */
#define CONTROL_CURRENT_PI_CONFIGURED               0U
#define CONTROL_SPEED_PI_CONFIGURED                 0U
#define CONTROL_COMMAND_RAMP_CONFIGURED             0U

// _Static_assert(BOARD_PWM_FREQUENCY_HZ ==
//                CONTROL_CURRENT_LOOP_FREQUENCY_HZ,
//                "The current loop must run once per PWM period");

// _Static_assert((CONTROL_CURRENT_LOOP_FREQUENCY_HZ %
//                 CONTROL_SPEED_LOOP_FREQUENCY_HZ) == 0U,
//                "The speed-loop frequency must divide the current-loop frequency");

#endif /* CONTROL_CONFIG_H */
