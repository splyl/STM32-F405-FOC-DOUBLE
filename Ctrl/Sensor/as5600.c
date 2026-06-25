#include "as5600.h"
#include "i2c.h"
#include <stdint.h>

#define AS5600_I2C_ADDR_HAL       (AS5600_I2C_ADDR_7BIT << 1)
#define AS5600_I2C_SCL_PORT       GPIOB
#define AS5600_I2C_SCL_PIN        GPIO_PIN_10
#define AS5600_I2C_SDA_PORT       GPIOB
#define AS5600_I2C_SDA_PIN        GPIO_PIN_11
#define AS5600_DMA_TIMEOUT_MS     2U

static volatile uint8_t as5600_busy = 0U;
static volatile uint8_t as5600_updated = 0U;
static volatile HAL_StatusTypeDef as5600_last_status = HAL_OK;
static volatile uint16_t as5600_latest_angle = 0U;
static volatile uint32_t as5600_dma_start_tick = 0U;
static uint8_t as5600_dma_buf[2] = {0};

static void as5600_delay(void)
{
    for (volatile uint32_t i = 0; i < 300U; ++i) {
        __NOP();
    }
}

static void as5600_i2c_bus_recover(void)
{
    GPIO_InitTypeDef gpio = {0};

    HAL_I2C_DeInit(&hi2c2);

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin = AS5600_I2C_SCL_PIN | AS5600_I2C_SDA_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);

    HAL_GPIO_WritePin(AS5600_I2C_SDA_PORT, AS5600_I2C_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AS5600_I2C_SCL_PORT, AS5600_I2C_SCL_PIN, GPIO_PIN_SET);
    as5600_delay();

    for (uint8_t i = 0; i < 9U; ++i) {
        HAL_GPIO_WritePin(AS5600_I2C_SCL_PORT, AS5600_I2C_SCL_PIN, GPIO_PIN_RESET);
        as5600_delay();
        HAL_GPIO_WritePin(AS5600_I2C_SCL_PORT, AS5600_I2C_SCL_PIN, GPIO_PIN_SET);
        as5600_delay();
    }

    HAL_GPIO_WritePin(AS5600_I2C_SDA_PORT, AS5600_I2C_SDA_PIN, GPIO_PIN_RESET);
    as5600_delay();
    HAL_GPIO_WritePin(AS5600_I2C_SCL_PORT, AS5600_I2C_SCL_PIN, GPIO_PIN_SET);
    as5600_delay();
    HAL_GPIO_WritePin(AS5600_I2C_SDA_PORT, AS5600_I2C_SDA_PIN, GPIO_PIN_SET);
    as5600_delay();

    MX_I2C2_Init();
}

//读取某一寄存器的数据
static HAL_StatusTypeDef read_u16_12bit(uint8_t reg_h,uint16_t *out)//读取两字节的低12位数据（查找芯片手册）
{
    uint8_t buf[2] = {0};
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(&hi2c2,
                                            AS5600_I2C_ADDR_HAL,
                                            reg_h,
                                            I2C_MEMADD_SIZE_8BIT,
                                            buf,
                                            2,
                                            10);
    if (rc == HAL_BUSY) {
        as5600_i2c_bus_recover();
        rc = HAL_I2C_Mem_Read(&hi2c2,
                              AS5600_I2C_ADDR_HAL,
                              reg_h,
                              I2C_MEMADD_SIZE_8BIT,
                              buf,
                              2,
                              10);
    }
    if (rc != HAL_OK) return rc;
    *out = ((uint16_t)(buf[0] & 0x0F) << 8 )|buf[1];
    return HAL_OK;
}

HAL_StatusTypeDef AS5600_read_angle(uint16_t *angle12)
{
    return read_u16_12bit(AS5600_REG_ANGLE_H, angle12);
}

HAL_StatusTypeDef AS5600_read_raw_angle12(uint16_t *raw12)
{
    return read_u16_12bit(AS5600_REG_RAW_ANGLE_H, raw12);
}

HAL_StatusTypeDef AS5600_read_status(uint8_t *st)
{
    HAL_StatusTypeDef rc = HAL_I2C_Mem_Read(&hi2c2,
                                            AS5600_I2C_ADDR_HAL,
                                            AS5600_REG_STATUS,
                                            I2C_MEMADD_SIZE_8BIT,
                                            st,
                                            1,
                                            10);
    if (rc == HAL_BUSY) {
        as5600_i2c_bus_recover();
        rc = HAL_I2C_Mem_Read(&hi2c2,
                              AS5600_I2C_ADDR_HAL,
                              AS5600_REG_STATUS,
                              I2C_MEMADD_SIZE_8BIT,
                              st,
                              1,
                              10);
    }

    return rc;
}

HAL_StatusTypeDef AS5600_read_agc(uint8_t *agc)
{
    return HAL_I2C_Mem_Read(&hi2c2,
                            AS5600_I2C_ADDR_HAL,
                            AS5600_REG_AGC,
                            I2C_MEMADD_SIZE_8BIT,
                            agc,
                            1,
                            10);
}

HAL_StatusTypeDef AS5600_read_magnitude12(uint16_t *mag12)
{
    return read_u16_12bit(AS5600_REG_MAG_H, mag12);
}

HAL_StatusTypeDef AS5600_StartReadAngleDMA(void)
{
    HAL_StatusTypeDef rc;

    if (as5600_busy != 0U) {
        return HAL_BUSY;
    }

    as5600_busy = 1U;
    as5600_updated = 0U;
    as5600_dma_start_tick = HAL_GetTick();
    rc = HAL_I2C_Mem_Read_DMA(&hi2c2,
                              AS5600_I2C_ADDR_HAL,
                              AS5600_REG_ANGLE_H,
                              I2C_MEMADD_SIZE_8BIT,
                              as5600_dma_buf,
                              2);
    if (rc != HAL_OK) {
        as5600_busy = 0U;
        as5600_last_status = rc;
    }

    return rc;
}

void AS5600_Service(void)
{
    if ((as5600_busy != 0U) && ((HAL_GetTick() - as5600_dma_start_tick) > AS5600_DMA_TIMEOUT_MS)) {
        as5600_i2c_bus_recover();
        as5600_busy = 0U;
        as5600_last_status = HAL_TIMEOUT;
    }
}

uint8_t AS5600_TakeAngle(uint16_t *angle12)
{
    if (angle12 == NULL) {
        return 0U;
    }

    __disable_irq();
    if (as5600_updated == 0U) {
        __enable_irq();
        return 0U;
    }

    *angle12 = as5600_latest_angle;
    as5600_updated = 0U;
    __enable_irq();

    return 1U;
}

uint8_t AS5600_IsAngleUpdated(void)
{
    uint8_t updated = as5600_updated;
    as5600_updated = 0U;
    return updated;
}

uint8_t AS5600_IsBusy(void)
{
    return as5600_busy;
}

uint16_t AS5600_GetAngle(void)
{
    return as5600_latest_angle;
}

HAL_StatusTypeDef AS5600_GetLastStatus(void)
{
    return as5600_last_status;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C2) {
        as5600_latest_angle = ((uint16_t)(as5600_dma_buf[0] & 0x0F) << 8) | as5600_dma_buf[1];
        as5600_last_status = HAL_OK;
        as5600_busy = 0U;
        as5600_updated = 1U;
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C2) {
        as5600_last_status = HAL_ERROR;
        as5600_busy = 0U;
    }
}
