#include "PCA9574.h"
#include <assert.h>

#include "esp_log.h"

#define TAG "PCA"

static struct {
    PCA9574_i2c_read i2c_read_fnc;
    PCA9574_i2c_write i2c_write_fnc;
    uint8_t dev_address;
} gb;

bool PCA9574_init(PCA9574_config_t *cfg) {
    assert(cfg->i2c_read_fnc != NULL);
    assert(cfg->i2c_write_fnc != NULL);
    if (cfg->i2c_read_fnc == NULL || cfg->i2c_write_fnc == NULL) {
        ESP_LOGE(TAG, "Null");
        return false;
    }

    gb.i2c_read_fnc = cfg->i2c_read_fnc;
    gb.i2c_write_fnc = cfg->i2c_write_fnc;
    gb.dev_address = cfg->dev_address;

    uint8_t input_port = 0;
    if (gb.i2c_read_fnc(gb.dev_address, PCA9574_INPUT_PORT_REG, &input_port, 1) == false) {
        ESP_LOGE(TAG, "Error :C");
        return false;
    }

    ESP_LOGI(TAG, "Input port register -> %d", input_port);
    return true;
}

bool PCA9574_set_mode(PCA9574_pin_mode_t mode) {
    uint8_t reg = 0x00;

    if (mode == PCA9574_INPUT) {
        reg = 0xFF;
    }

    if (gb.i2c_write_fnc(gb.dev_address, PCA9574_CONFIG_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCA9574_set_mode_pin(PCA9574_pin_mode_t mode, uint8_t pin) {
    uint8_t reg = 0x00;
    if (gb.i2c_read_fnc(gb.dev_address, PCA9574_CONFIG_REG, &reg, 1) == false) {
        return false;
    }

    if (mode == PCA9574_OUTPUT) {
        reg &= ~(mode << pin);
    } else {
        reg |= (mode << pin);
    }

    if (gb.i2c_write_fnc(gb.dev_address, PCA9574_CONFIG_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCA9574_set_level(PCA9574_pin_level_t level) {
    uint8_t reg = 0x00;

    if (level == PCA9574_HIGH) {
        reg = 0xFF;
    }

    if (gb.i2c_write_fnc(gb.dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCA9574_set_level_pin(PCA9574_pin_level_t level, uint8_t pin) {
    uint8_t reg = 0x00;
    if (gb.i2c_read_fnc(gb.dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }
    ESP_LOGI(TAG, "PIN %d", reg);

    if (level == PCA9574_LOW) {
        reg &= ~(level << pin);
    } else {
        reg |= (level << pin);
    }

    if (gb.i2c_write_fnc(gb.dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    if (gb.i2c_read_fnc(gb.dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }
    ESP_LOGI(TAG, "PIN 2 %d", reg);
    return true;
}