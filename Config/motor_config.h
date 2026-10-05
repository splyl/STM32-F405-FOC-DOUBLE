#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

/* Motor-1 parameters currently known ------------------------------------- */
#define MOTOR1_POLE_PAIRS                         7U
#define MOTOR1_NOMINAL_BUS_VOLTAGE_V            12.0f
#define MOTOR1_MAX_MECHANICAL_SPEED_RPM       3000.0f
#define MOTOR1_MAX_PHASE_CURRENT_A               5.0f

/* Derived limits ---------------------------------------------------------- */
#define MOTOR_RPM_TO_RAD_PER_SEC       0.1047197551f
#define MOTOR1_MAX_MECHANICAL_SPEED_RAD_PER_SEC \
    (MOTOR1_MAX_MECHANICAL_SPEED_RPM * MOTOR_RPM_TO_RAD_PER_SEC)
#define MOTOR1_MAX_ELECTRICAL_SPEED_RAD_PER_SEC \
    (MOTOR1_MAX_MECHANICAL_SPEED_RAD_PER_SEC * (float)MOTOR1_POLE_PAIRS)

/*
 * These parameters are not known yet.  A closed current loop must not be
 * enabled until the current-sense scale has been measured and configured.
 */
#define MOTOR1_CURRENT_SENSE_CONFIGURED            0U
#define MOTOR1_ELECTRICAL_PARAMETERS_CONFIGURED    0U
#define MOTOR1_ENCODER_ZERO_CONFIGURED              0U

#endif /* MOTOR_CONFIG_H */
