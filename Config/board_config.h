
/* MCU clocks -------------------------------------------------------------- */
#define BOARD_SYSTEM_CLOCK_HZ              168000000U
#define BOARD_APB1_CLOCK_HZ                 42000000U
#define BOARD_APB1_TIMER_CLOCK_HZ           84000000U
#define BOARD_APB2_CLOCK_HZ                 84000000U
#define BOARD_APB2_TIMER_CLOCK_HZ          168000000U

/* Motor PWM timing ------------------------------------------------------ */
#define BOARD_PWM_FREQUENCY_HZ                 20000U
#define BOARD_PWM_TIMER_CLOCK_HZ            84000000U
#define BOARD_PWM_TIMER_PRESCALER                   0U
#define BOARD_PWM_TIMER_PERIOD_COUNTS            2100U
#define BOARD_PWM_TIMER_AUTORELOAD              2099U

/*
 * TIM2 uses center-aligned PWM mode 2.  The control layer expresses duty as
 * the desired high-side on-time in the range 0.0f to 1.0f.  The platform PWM
 * adapter must therefore convert it to an inverted compare value.
 */
#define BOARD_PWM_COMPARE_IS_INVERTED              1U
#define BOARD_PWM_NEUTRAL_DUTY                   0.5f

/* Control-event rates produced by the configured peripherals ------------ */
#define BOARD_ADC_TRIGGER_FREQUENCY_HZ          20000U
#define BOARD_SPEED_TIMER_FREQUENCY_HZ           1000U

/* ADC characteristics ---------------------------------------------------- */
#define BOARD_ADC_RESOLUTION_BITS                  12U
#define BOARD_ADC_MAX_COUNT                      4095U

/* Nominal power supply ---------------------------------------------------- */
#define BOARD_NOMINAL_BUS_VOLTAGE_V              12.0f