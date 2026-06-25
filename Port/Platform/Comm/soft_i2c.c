#include "soft_i2c.h"
#include "stm32f405xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "system_stm32f4xx.h"
#include <stdint.h>

// ms 延时

const si2c_bus_t si2c_bus_m1_encoder = {
    .scl_port = M1_Encoder_SCL_GPIO_Port,
    .scl_pin = M1_Encoder_SCL_Pin,
    .sda_port = M1_Encoder_SDA_GPIO_Port,
    .sda_pin = M1_Encoder_SDA_Pin,
};

const si2c_bus_t si2c_bus_m2_encoder = {
    .scl_port = M2_Encoder_SCL_GPIO_Port,
    .scl_pin = M2_Encoder_SCL_Pin,
    .sda_port = M2_Encoder_SDA_GPIO_Port,
    .sda_pin = M2_Encoder_SDA_Pin,
};

static const si2c_bus_t *si2c_active_bus = &si2c_bus_m1_encoder;

static const si2c_bus_t *si2c_get_active_bus(void)
{
    return (si2c_active_bus != NULL) ? si2c_active_bus : &si2c_bus_m1_encoder;
}

void si2c_set_bus(const si2c_bus_t *bus)
{
    si2c_active_bus = (bus != NULL) ? bus : &si2c_bus_m1_encoder;
}

const si2c_bus_t *si2c_get_bus(void)
{
    return si2c_get_active_bus();
}



static void dwt_delay_init(void)
{
    // 使能 DWT CYCCNT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us( uint32_t us)
{
    // DWT延时
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us*(SystemCoreClock/1000000U);
    while ((DWT->CYCCNT-start)<ticks) {__NOP();}
}
static inline void si2c_delay_us(void) {delay_us(SI2C_HALF_PERIOD_US);}
static void SDA_OUT_OD(const si2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = bus->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(bus->sda_port,&GPIO_InitStruct);
}
static void SDA_IN(const si2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = bus->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(bus->sda_port,&GPIO_InitStruct);   
}
static void SCL_OUT_OD(const si2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = bus->scl_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(bus->scl_port,&GPIO_InitStruct);
}

static inline void SCL_H(const si2c_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->scl_port, bus->scl_pin,GPIO_PIN_SET);
}
static inline void SCL_L(const si2c_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->scl_port, bus->scl_pin,GPIO_PIN_RESET);
}
static inline void SDA_H(const si2c_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->sda_port, bus->sda_pin,GPIO_PIN_SET);
}
static inline void SDA_L(const si2c_bus_t *bus)
{
    HAL_GPIO_WritePin(bus->sda_port, bus->sda_pin,GPIO_PIN_RESET);
}

static inline uint8_t SDA_READ(const si2c_bus_t *bus)
{
    return (HAL_GPIO_ReadPin(bus->sda_port, bus->sda_pin) == GPIO_PIN_SET) ? 1U : 0U;
}
static inline uint8_t SCL_READ(const si2c_bus_t *bus)
{
    // 并不需要设置成输入模式，其读取的是目前SCL引脚的状态
    return (HAL_GPIO_ReadPin(bus->scl_port, bus->scl_pin) == GPIO_PIN_SET) ? 1U : 0U;
}

static uint8_t wait_scl_hith(const si2c_bus_t *bus, uint32_t timeout_us)
{
    // 判断是否设置成了高电平
    SCL_H(bus);
    while(!SCL_READ(bus)){
        if(timeout_us-- == 0) return 0;
        delay_us( 1 );
    }
    return 1;
}
void si2c_init_bus(const si2c_bus_t *bus)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    dwt_delay_init();//DWT初始化
    SCL_OUT_OD(bus);
    SDA_OUT_OD(bus);
    SDA_H(bus);
    SCL_H(bus);
    delay_us(10);
    //强制释放总线信号
    si2c_bus_recover_bus(bus);
}

void si2c_start_bus(const si2c_bus_t *bus)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    // 起始标志，在SCL为高电平时，SDA产生一个下降沿
    SDA_OUT_OD(bus);
    SDA_H(bus);
    SCL_H(bus);
    si2c_delay_us();

    SDA_L(bus);
    si2c_delay_us();

    SCL_L(bus);
    si2c_delay_us();
}
void si2c_stop_bus(const si2c_bus_t *bus)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    // 结束标志，在SCL为高电平时，SDA产生一个上升沿
    SDA_OUT_OD(bus);
    SDA_L(bus);
    si2c_delay_us();

    SCL_H(bus);
    si2c_delay_us();

    SDA_H(bus);
    si2c_delay_us();
}
// 写字节
uint8_t si2c_write_byte_bus(const si2c_bus_t *bus, uint8_t byte)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    // 先将SDA设置为输出
    SDA_OUT_OD(bus);
    SDA_H(bus);
    for (int i = 0; i < 8;  i++) {
        SCL_L(bus);
        si2c_delay_us();
        (byte & 0x80)? SDA_H(bus) : SDA_L(bus);
        byte <<=1;
        si2c_delay_us();
        // 判断SCL状态，如果不为1，返回错误状态0，
        if (! wait_scl_hith(bus, 100)) {
            SCL_L(bus);
            return 0;
        }
        si2c_delay_us();   
    }
    // 写完一个字节，等待应答
    SCL_L(bus);
    si2c_delay_us();
    SDA_IN(bus);
    si2c_delay_us();
    // 
    if(!wait_scl_hith(bus, 100)){
        SDA_L(bus);
        SDA_OUT_OD(bus);
        SDA_H(bus);
        return 0;
    }
    si2c_delay_us();
    uint8_t ack = (SDA_READ(bus)==0U)?1U:0U;
    // 读取完成，释放总线，并返回应答状态
    SCL_L(bus);
    si2c_delay_us();
    SDA_OUT_OD(bus);
    SDA_H(bus);
    return ack;
}
// 读字节
uint8_t si2c_read_byte_bus(const si2c_bus_t *bus, uint8_t ack)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    uint8_t byte = 0;
    // 从机写,主机读
    SDA_IN(bus);
    for (int i=0; i<8; i++) {
        SCL_L(bus);
        si2c_delay_us();
        if (!wait_scl_hith(bus, 100)) {
            SCL_L(bus);
            return 0;
        }
        byte<<=1;
        if (SDA_READ(bus)) {
            byte|=0x01;
        }
    }
    // 读取一个字节完成,主机发送ask
    SCL_L(bus);
    si2c_delay_us();
    SDA_OUT_OD(bus);

    if (ack) SDA_L(bus);
    else SDA_H(bus);
    if (!wait_scl_hith(bus, 100)) {
        SCL_L(bus);
        SDA_H(bus);
        return byte;
    }
    // 释放总线,并返回byte
    si2c_delay_us();
    SCL_L(bus);
    si2c_delay_us();
    SDA_H(bus);
    return byte;
}
// 写寄存器
si2c_status_t si2c_write_reg_bus(const si2c_bus_t *bus, uint8_t dev7, uint8_t reg, uint8_t *data, uint16_t len)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    si2c_start_bus(bus);
    if(!si2c_write_byte_bus(bus, dev7<<1|0x00)){si2c_stop_bus(bus);return SI2C_ERR_NACK;}
    if(!si2c_write_byte_bus(bus, reg)){si2c_stop_bus(bus);return SI2C_ERR_NACK;}

    for (uint16_t i =0 ;i<len; i++){
        if (!si2c_write_byte_bus(bus, data[i])) { si2c_stop_bus(bus); return SI2C_ERR_NACK; }
    }
    si2c_stop_bus(bus);
    return SI2C_OK;
}
// 读寄存器
si2c_status_t si2c_read_reg_bus(const si2c_bus_t *bus, uint8_t dev7, uint8_t reg, uint8_t *data, uint16_t len)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    // 需要先写要读取的寄存器地址
    si2c_start_bus(bus);
    if(!si2c_write_byte_bus(bus, dev7<<1|0x00)){si2c_stop_bus(bus);return SI2C_ERR_NACK;}
    if(!si2c_write_byte_bus(bus, reg)){si2c_stop_bus(bus);return SI2C_ERR_NACK;}
    // 再次开启I2C并设置成读模式
    si2c_start_bus(bus);
    if(!si2c_write_byte_bus(bus, (dev7<<1)|0x01)){si2c_stop_bus(bus);return SI2C_ERR_NACK;}
    for (uint16_t i=0; i<len;i++) {
        // 判断是否是最后一位
        data[i] =  si2c_read_byte_bus(bus, i!= (len-1)?1:0);
    }
    si2c_stop_bus(bus);
    return SI2C_OK;
}
void si2c_bus_recover_bus(const si2c_bus_t *bus)
{
    if (bus == NULL) {
        bus = si2c_get_active_bus();
    }
    //主机的SDA很容易被从机的SDA给拉低,所以,才使用SCL脉冲使得从机释放总线信号
    SDA_IN(bus);
    SCL_OUT_OD(bus);
    SCL_H(bus);
    delay_us(5);
    if (SDA_READ(bus)==0U) {
        for(uint8_t i=0;i<9;i++)
        {
            SCL_L(bus);
            delay_us(5);
            SCL_H(bus);
            delay_us(5);
        }
    }    
    // 再产生一个STOP信号
    si2c_stop_bus(bus);
}

void si2c_init(void)
{
    si2c_init_bus(si2c_get_active_bus());
}

void si2c_start(void)
{
    si2c_start_bus(si2c_get_active_bus());
}

void si2c_stop(void)
{
    si2c_stop_bus(si2c_get_active_bus());
}

uint8_t si2c_write_byte(uint8_t byte)
{
    return si2c_write_byte_bus(si2c_get_active_bus(), byte);
}

uint8_t si2c_read_byte(uint8_t ack)
{
    return si2c_read_byte_bus(si2c_get_active_bus(), ack);
}

si2c_status_t si2c_write_reg(uint8_t dev7,uint8_t reg,uint8_t *data,uint16_t len)
{
    return si2c_write_reg_bus(si2c_get_active_bus(), dev7, reg, data, len);
}

si2c_status_t si2c_read_reg(uint8_t dev7,uint8_t reg,uint8_t *data,uint16_t len)
{
    return si2c_read_reg_bus(si2c_get_active_bus(), dev7, reg, data, len);
}

void si2c_bus_recover(void)
{
    si2c_bus_recover_bus(si2c_get_active_bus());
}
