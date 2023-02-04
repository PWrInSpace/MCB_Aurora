// Copyright 2022 PWrInSpace, Kuba
#ifndef I2C_SIMPLE
#define I2C_SIMPLE

#include "driver/i2c.h"

typedef i2c_config_t i2c_t;

bool I2C_master_init(i2c_t *i2c, i2c_port_t port, uint8_t sda, uint8_t scl);
bool I2C_master_write(i2c_port_t port, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                        size_t len);
bool I2C_master_read(i2c_port_t port, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                        size_t len);
bool i2c_num1_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);
bool i2c_num1_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

#endif
