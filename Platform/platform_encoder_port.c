#include "platform_encoder_port.h"

#include <stddef.h>

#define ENCODER_I2C_RECOVERY_CLOCK_PULSES    9U
#define ENCODER_I2C_RECOVERY_DELAY_CYCLES  300U

typedef struct
{
    const MotorHardwareBinding *hardware;
    I2C_HandleTypeDef *i2c;
    bool initialized;
} PlatformEncoderPortContext;

static PlatformEncoderPortContext encoder_port_context;

static void PlatformEncoderPort_Delay(void)
{
    volatile uint32_t cycle;

    for (cycle = 0U;
         cycle < ENCODER_I2C_RECOVERY_DELAY_CYCLES;
         cycle++)
    {
        __NOP();
    }
}

static void PlatformEncoderPort_ConfigureRecoveryPins(
    const MotorHardwareBinding *hardware)
{
    GPIO_InitTypeDef gpio = {0};

    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    if (hardware->encoder_scl_port == hardware->encoder_sda_port)
    {
        gpio.Pin = hardware->encoder_scl_pin |
                   hardware->encoder_sda_pin;
        HAL_GPIO_Init(hardware->encoder_scl_port, &gpio);
    }
    else
    {
        gpio.Pin = hardware->encoder_scl_pin;
        HAL_GPIO_Init(hardware->encoder_scl_port, &gpio);

        gpio.Pin = hardware->encoder_sda_pin;
        HAL_GPIO_Init(hardware->encoder_sda_port, &gpio);
    }
}

static bool PlatformEncoderPort_RecoverBus(
    const MotorHardwareBinding *hardware)
{
    I2C_HandleTypeDef *i2c;
    uint32_t pulse;

    i2c = hardware->encoder_i2c;

    if (HAL_I2C_DeInit(i2c) != HAL_OK)
    {
        return false;
    }

    PlatformEncoderPort_ConfigureRecoveryPins(hardware);

    HAL_GPIO_WritePin(hardware->encoder_sda_port,
                      hardware->encoder_sda_pin,
                      GPIO_PIN_SET);
    HAL_GPIO_WritePin(hardware->encoder_scl_port,
                      hardware->encoder_scl_pin,
                      GPIO_PIN_SET);
    PlatformEncoderPort_Delay();

    for (pulse = 0U;
         pulse < ENCODER_I2C_RECOVERY_CLOCK_PULSES;
         pulse++)
    {
        HAL_GPIO_WritePin(hardware->encoder_scl_port,
                          hardware->encoder_scl_pin,
                          GPIO_PIN_RESET);
        PlatformEncoderPort_Delay();
        HAL_GPIO_WritePin(hardware->encoder_scl_port,
                          hardware->encoder_scl_pin,
                          GPIO_PIN_SET);
        PlatformEncoderPort_Delay();
    }

    /* Generate a STOP condition: SDA low-to-high while SCL is high. */
    HAL_GPIO_WritePin(hardware->encoder_sda_port,
                      hardware->encoder_sda_pin,
                      GPIO_PIN_RESET);
    PlatformEncoderPort_Delay();
    HAL_GPIO_WritePin(hardware->encoder_scl_port,
                      hardware->encoder_scl_pin,
                      GPIO_PIN_SET);
    PlatformEncoderPort_Delay();
    HAL_GPIO_WritePin(hardware->encoder_sda_port,
                      hardware->encoder_sda_pin,
                      GPIO_PIN_SET);
    PlatformEncoderPort_Delay();

    if (HAL_I2C_Init(i2c) != HAL_OK)
    {
        return false;
    }

    return (__HAL_I2C_GET_FLAG(i2c, I2C_FLAG_BUSY) == RESET) &&
           (HAL_I2C_GetState(i2c) == HAL_I2C_STATE_READY);
}

bool PlatformEncoderPort_Init(const MotorHardwareBinding *hardware)
{
    I2C_HandleTypeDef *i2c;

    if ((hardware == NULL) ||
        (hardware->encoder_i2c == NULL) ||
        (hardware->encoder_i2c->Instance == NULL) ||
        (hardware->encoder_i2c->hdmarx == NULL) ||
        (hardware->encoder_i2c->hdmatx == NULL) ||
        (hardware->encoder_scl_port == NULL) ||
        (hardware->encoder_sda_port == NULL) ||
        (hardware->encoder_scl_pin == 0U) ||
        (hardware->encoder_sda_pin == 0U))
    {
        return false;
    }

    encoder_port_context.hardware = NULL;
    encoder_port_context.i2c = NULL;
    encoder_port_context.initialized = false;

    i2c = hardware->encoder_i2c;
    if ((HAL_I2C_GetState(i2c) != HAL_I2C_STATE_READY) ||
        (__HAL_I2C_GET_FLAG(i2c, I2C_FLAG_BUSY) != RESET))
    {
        if (!PlatformEncoderPort_RecoverBus(hardware))
        {
            return false;
        }
    }

    encoder_port_context.hardware = hardware;
    encoder_port_context.i2c = i2c;
    encoder_port_context.initialized = true;

    return true;
}

bool PlatformEncoderPort_Recover(void)
{
    const MotorHardwareBinding *hardware;

    hardware = encoder_port_context.hardware;
    if ((hardware == NULL) || (encoder_port_context.i2c == NULL))
    {
        return false;
    }

    encoder_port_context.initialized = false;
    if (!PlatformEncoderPort_RecoverBus(hardware))
    {
        return false;
    }

    encoder_port_context.i2c = hardware->encoder_i2c;
    encoder_port_context.initialized = true;
    return true;
}

HAL_StatusTypeDef PlatformEncoderPort_IsDeviceReady(
    uint16_t device_address,
    uint32_t trials,
    uint32_t timeout_ms)
{
    if ((!encoder_port_context.initialized) ||
        (encoder_port_context.i2c == NULL))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_IsDeviceReady(encoder_port_context.i2c,
                                 device_address,
                                 trials,
                                 timeout_ms);
}

HAL_StatusTypeDef PlatformEncoderPort_MemRead(
    uint16_t device_address,
    uint16_t register_address,
    uint16_t register_address_size,
    uint8_t *data,
    uint16_t length,
    uint32_t timeout_ms)
{
    HAL_StatusTypeDef status;

    if ((!encoder_port_context.initialized) ||
        (encoder_port_context.i2c == NULL) ||
        (data == NULL) ||
        (length == 0U))
    {
        return HAL_ERROR;
    }

    status = HAL_I2C_Mem_Read(encoder_port_context.i2c,
                              device_address,
                              register_address,
                              register_address_size,
                              data,
                              length,
                              timeout_ms);

    if ((status == HAL_BUSY) && PlatformEncoderPort_Recover())
    {
        status = HAL_I2C_Mem_Read(encoder_port_context.i2c,
                                  device_address,
                                  register_address,
                                  register_address_size,
                                  data,
                                  length,
                                  timeout_ms);
    }

    return status;
}

HAL_StatusTypeDef PlatformEncoderPort_MemReadDma(
    uint16_t device_address,
    uint16_t register_address,
    uint16_t register_address_size,
    uint8_t *data,
    uint16_t length)
{
    if ((!encoder_port_context.initialized) ||
        (encoder_port_context.i2c == NULL) ||
        (data == NULL) ||
        (length == 0U))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Read_DMA(encoder_port_context.i2c,
                                device_address,
                                register_address,
                                register_address_size,
                                data,
                                length);
}

__weak void PlatformEncoderPort_RxCompleteCallback(void)
{
}

__weak void PlatformEncoderPort_ErrorCallback(void)
{
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *i2c)
{
    if (encoder_port_context.initialized &&
        (i2c == encoder_port_context.i2c))
    {
        PlatformEncoderPort_RxCompleteCallback();
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *i2c)
{
    if (encoder_port_context.initialized &&
        (i2c == encoder_port_context.i2c))
    {
        PlatformEncoderPort_ErrorCallback();
    }
}
