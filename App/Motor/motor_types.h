#ifndef MOTOR_TYPES_H
#define MOTOR_TYPES_H

typedef struct MotorHardwareBinding MotorHardwareBinding;

typedef enum
{
    MOTOR_ID_1 = 0,
    MOTOR_ID_2 = 1,
    MOTOR_ID_COUNT
} MotorId;

typedef enum
{
    MOTOR_STATE_DISABLED = 0,  /* Motor instance is not enabled. */
    MOTOR_STATE_BOOT,          /* Software object has just been initialized. */
    MOTOR_STATE_IDLE,          /* Ready to accept a start command. */
    MOTOR_STATE_START_REQUEST, /* A start command has been accepted. */
    MOTOR_STATE_STARTING,      /* Start sequence is in progress. */
    MOTOR_STATE_RUN,           /* Motor is in the normal running state. */
    MOTOR_STATE_STOP_REQUEST,  /* A stop command has been accepted. */
    MOTOR_STATE_STOPPING,      /* Stop sequence is in progress. */
    MOTOR_STATE_FAULT          /* A fault is active or latched. */
} MotorState;

typedef enum
{
    MOTOR_COMMAND_NONE = 0,
    MOTOR_COMMAND_START,
    MOTOR_COMMAND_STOP
} MotorCommand;

typedef struct
{
    MotorId id;
    volatile MotorState state;
    volatile MotorCommand pending_command;
    const MotorHardwareBinding *hardware;
} MotorControl;

#endif /* MOTOR_TYPES_H */
