#ifndef MY_I2C_H_
#define MY_I2C_H_
#include "stm32f10x.h"
#include "my_gpio.h"

//i2c channels
#define I2C1_CH 1
#define I2C2_CH 2

//i2c ports
#define I2C1_SCL PB6
#define I2C1_SDA PB7
#define I2C2_SCL PB10
#define I2C2_SDA PB11

void i2cCONFIG(uint8_t ch);
void i2c1WRITE(uint8_t addr,uint8_t byte);
void i2c2WRITE(uint8_t addr,uint8_t byte);

#endif
