#include "as5600.h"

#include <stddef.h>

#include "platform_encoder_port.h"

#define AS5600_I2C_ADDRESS_HAL       (AS5600_I2C_ADDRESS_7BIT << 1U)
#define AS5600_TRANSFER_TIMEOUT_MS   10U
#define AS5600_DMA_TIMEOUT_MS         2U
#define AS5600_READY_TRIALS           3U

typedef struct
{
    volatile bool initialized;
    volatile bool busy;
    volatile bool angle_updated;
    volatile HAL_StatusTypeDef last_status;
    volatile uint16_t latest_angle;
    volatile uint32_t dma_start_tick;
    uint8_t dma_buffer[2];
} AS5600_Context;

static AS5600_Context as5600_context;

static HAL_StatusTypeDef AS5600_Read12BitRegister(uint8_t high_register,
                                                  uint16_t *value)
{
    HAL_StatusTypeDef status;
    uint8_t buffer[2] = {0U, 0U};

    if (value == NULL)
    {
        return HAL_ERROR;
    }

    status = PlatformEncoderPort_MemRead(AS5600_I2C_ADDRESS_HAL,
                                         high_register,
                                         I2C_MEMADD_SIZE_8BIT,
                                         buffer,
                                         2U,
                                         AS5600_TRANSFER_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        return status;
    }

    *value = ((uint16_t)(buffer[0] & 0x0FU) << 8U) |
             (uint16_t)buffer[1];
    return HAL_OK;
}

bool AS5600_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t sensor_status;

    as5600_context.initialized = false;
    as5600_context.busy = false;
    as5600_context.angle_updated = false;
    as5600_context.last_status = HAL_ERROR;
    as5600_context.latest_angle = 0U;
    as5600_context.dma_start_tick = 0U;
    as5600_context.dma_buffer[0] = 0U;
    as5600_context.dma_buffer[1] = 0U;

    status = PlatformEncoderPort_IsDeviceReady(AS5600_I2C_ADDRESS_HAL,
                                               AS5600_READY_TRIALS,
                                               AS5600_TRANSFER_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        as5600_context.last_status = status;
        return false;
    }

    status = AS5600_ReadStatus(&sensor_status);
    if (status != HAL_OK)
    {
        as5600_context.last_status = status;
        return false;
    }

    as5600_context.last_status = HAL_OK;
    as5600_context.initialized = true;
    return true;
}

HAL_StatusTypeDef AS5600_ReadAngle(uint16_t *angle_12bit)
{
    return AS5600_Read12BitRegister(AS5600_REG_ANGLE_H, angle_12bit);
}

HAL_StatusTypeDef AS5600_ReadRawAngle(uint16_t *angle_12bit)
{
    return AS5600_Read12BitRegister(AS5600_REG_RAW_ANGLE_H, angle_12bit);
}

HAL_StatusTypeDef AS5600_ReadStatus(uint8_t *status)
{
    if (status == NULL)
    {
        return HAL_ERROR;
    }

    return PlatformEncoderPort_MemRead(AS5600_I2C_ADDRESS_HAL,
                                       AS5600_REG_STATUS,
                                       I2C_MEMADD_SIZE_8BIT,
                                       status,
                                       1U,
                                       AS5600_TRANSFER_TIMEOUT_MS);
}

HAL_StatusTypeDef AS5600_ReadAgc(uint8_t *agc)
{
    if (agc == NULL)
    {
        return HAL_ERROR;
    }

    return PlatformEncoderPort_MemRead(AS5600_I2C_ADDRESS_HAL,
                                       AS5600_REG_AGC,
                                       I2C_MEMADD_SIZE_8BIT,
                                       agc,
                                       1U,
                                       AS5600_TRANSFER_TIMEOUT_MS);
}

HAL_StatusTypeDef AS5600_ReadMagnitude(uint16_t *magnitude_12bit)
{
    return AS5600_Read12BitRegister(AS5600_REG_MAGNITUDE_H,
                                    magnitude_12bit);
}

HAL_StatusTypeDef AS5600_StartReadAngleDma(void)
{
    HAL_StatusTypeDef status;

    if (!as5600_context.initialized)
    {
        return HAL_ERROR;
    }

    if (as5600_context.busy)
    {
        return HAL_BUSY;
    }

    as5600_context.busy = true;
    as5600_context.angle_updated = false;
    as5600_context.dma_start_tick = HAL_GetTick();

    status = PlatformEncoderPort_MemReadDma(AS5600_I2C_ADDRESS_HAL,
                                            AS5600_REG_ANGLE_H,
                                            I2C_MEMADD_SIZE_8BIT,
                                            as5600_context.dma_buffer,
                                            2U);
    if (status != HAL_OK)
    {
        as5600_context.busy = false;
        as5600_context.last_status = status;
    }

    return status;
}

void AS5600_Service(void)
{
    if (as5600_context.busy &&
        ((HAL_GetTick() - as5600_context.dma_start_tick) >
         AS5600_DMA_TIMEOUT_MS))
    {
        (void)PlatformEncoderPort_Recover();
        as5600_context.busy = false;
        as5600_context.last_status = HAL_TIMEOUT;
    }
}

bool AS5600_TakeAngle(uint16_t *angle_12bit)
{
    uint32_t interrupt_mask;

    if (angle_12bit == NULL)
    {
        return false;
    }

    interrupt_mask = __get_PRIMASK();
    __disable_irq();

    if (!as5600_context.angle_updated)
    {
        if (interrupt_mask == 0U)
        {
            __enable_irq();
        }
        return false;
    }

    *angle_12bit = as5600_context.latest_angle;
    as5600_context.angle_updated = false;

    if (interrupt_mask == 0U)
    {
        __enable_irq();
    }

    return true;
}

bool AS5600_IsAngleUpdated(void)
{
    return as5600_context.angle_updated;
}

bool AS5600_IsBusy(void)
{
    return as5600_context.busy;
}

uint16_t AS5600_GetAngle(void)
{
    return as5600_context.latest_angle;
}

HAL_StatusTypeDef AS5600_GetLastStatus(void)
{
    return as5600_context.last_status;
}

void PlatformEncoderPort_RxCompleteCallback(void)
{
    as5600_context.latest_angle =
        ((uint16_t)(as5600_context.dma_buffer[0] & 0x0FU) << 8U) |
        (uint16_t)as5600_context.dma_buffer[1];
    as5600_context.last_status = HAL_OK;
    as5600_context.busy = false;
    as5600_context.angle_updated = true;
}

void PlatformEncoderPort_ErrorCallback(void)
{
    as5600_context.last_status = HAL_ERROR;
    as5600_context.busy = false;
}
