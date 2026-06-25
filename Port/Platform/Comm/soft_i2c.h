#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "gpio.h"
#include "main.h"

#define  SI2C_HALF_PERIOD_US 1

typedef enum {
    SI2C_OK = 0,
    SI2C_ERR_NACK = 1,
    SI2C_ERR_TIMEOUT = 2,
} si2c_status_t;

typedef struct {
    GPIO_TypeDef *scl_port;
    uint16_t scl_pin;
    GPIO_TypeDef *sda_port;
    uint16_t sda_pin;
} si2c_bus_t;

extern const si2c_bus_t si2c_bus_m1_encoder;
extern const si2c_bus_t si2c_bus_m2_encoder;

void si2c_set_bus(const si2c_bus_t *bus);
const si2c_bus_t *si2c_get_bus(void);

void si2c_init_bus(const si2c_bus_t *bus);

// 基础时序
void si2c_start_bus(const si2c_bus_t *bus);
void si2c_stop_bus(const si2c_bus_t *bus);
uint8_t si2c_write_byte_bus(const si2c_bus_t *bus, uint8_t byte); // return 1=ACK, 0=NACK
uint8_t si2c_read_byte_bus(const si2c_bus_t *bus, uint8_t ack);   // ack=1发送ACK；ack=0发送NACK

// 寄存器读写（8-bit reg address）
si2c_status_t si2c_read_reg_bus(const si2c_bus_t *bus, uint8_t dev7, uint8_t reg, uint8_t *data, uint16_t len);
si2c_status_t si2c_write_reg_bus(const si2c_bus_t *bus, uint8_t dev7, uint8_t reg, uint8_t *data, uint16_t len);
void si2c_bus_recover_bus(const si2c_bus_t *bus);

void si2c_init(void);

// 基础时序
void si2c_start(void);
void si2c_stop(void);
uint8_t si2c_write_byte(uint8_t byte);     // return 1=ACK, 0=NACK
uint8_t si2c_read_byte(uint8_t ack);       // ack=1发送ACK；ack=0发送NACK

// 寄存器读写（8-bit reg address）
si2c_status_t si2c_read_reg(uint8_t dev7,uint8_t reg,uint8_t *data,uint16_t len);
si2c_status_t si2c_write_reg(uint8_t dev7,uint8_t reg,uint8_t *data,uint16_t len);


void si2c_bus_recover(void);





#endif
