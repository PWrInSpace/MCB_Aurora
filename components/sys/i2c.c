// Copyright 2022 PWrInSpace, Kuba
#include "i2c.h"

bool I2C_master_init(i2c_t *i2c, i2c_port_t port, uint8_t sda, uint8_t scl) {
    esp_err_t res;
    i2c->mode = I2C_MODE_MASTER;
    i2c->sda_io_num = sda;
    i2c->scl_io_num = scl;
    i2c->sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c->scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c->master.clk_speed = 400000;
    i2c->clk_flags = 0;

    res = i2c_param_config(port, i2c);
    res = i2c_driver_install(port, i2c->mode, 0, 0, 0);
    return res == ESP_OK ? true : false;
}

bool I2C_master_read(i2c_port_t port, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                          size_t len) {
    if (i2c_master_write_read_device(port, dev_addr, &reg_addr, 1, data, len,
                                   pdMS_TO_TICKS(100)) == ESP_OK) {
        return true;
    }
    return false;
}

bool I2C_master_write(i2c_port_t port, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                           size_t len) {
    if (len <= 0) {
        return false;
    }

    uint8_t *tmp = (uint8_t *)malloc(sizeof(uint8_t) * len);
    tmp[0] = reg_addr;
    for (int i = 1; i < len; ++i) {  // TO DO: Change to memcpy
        tmp[i] = data[i];
    }
    if (i2c_master_write_to_device(port, dev_addr, tmp, sizeof(tmp),
                                 pdMS_TO_TICKS(100)) == ESP_OK) {
        free(tmp);
        return true;
    }

    free(tmp);
    return false;
}

bool i2c_num1_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                          size_t len) {
    if (I2C_master_read(I2C_NUM_1, dev_addr, reg_addr, data, len)) {
        return true;
    }

    return false;
}


bool i2c_num1_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                          size_t len) {
    if (I2C_master_write(I2C_NUM_1, dev_addr, reg_addr, data, len)) {
        return true;
    }

    return false;
}
