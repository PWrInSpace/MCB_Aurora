#include <assert.h>
#include "PCA9574.h"
#include "esp_log.h"


bool PCA9574_init(PCA9574_t *pca) {
    assert(pca->i2c_read_fnc != NULL);
    assert(pca->i2c_write_fnc != NULL);
    if (pca->i2c_read_fnc == NULL || pca->i2c_write_fnc == NULL) {
        return false;
    }

    uint8_t input_port = 0;
    if (pca->i2c_read_fnc(pca->dev_address, PCA9574_INPUT_PORT_REG, &input_port, 1) == false) {
        return false;
    }

    return true;
}

bool PCA9574_set_mode(PCA9574_t *pca, PCA9574_pin_mode_t mode) {
    uint8_t reg = 0x00;

    if (mode == PCA9574_INPUT) {
        reg = 0xFF;
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCA9574_CONFIG_REG, &reg, 1) == false) {
        return false;
    }

    // Disable high impedance (perhaps) on pin, idk why set to output does not set pins to low, but also pins are not at hight level
    if (mode == PCA9574_OUTPUT) {
        reg = 0x00;
        if (pca->i2c_write_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
            return false;
        }
    }

    return true;
}

bool PCA9574_set_mode_pin(PCA9574_t *pca, PCA9574_pin_mode_t mode, uint8_t pin) {
    uint8_t reg = 0x00;
    if (pca->i2c_read_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    if (mode == PCA9574_OUTPUT) {
        reg &= ~(mode << pin);
    } else {
        reg |= (mode << pin);
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCA9574_set_level(PCA9574_t *pca, PCA9574_pin_level_t level) {
    uint8_t reg = 0x00;

    if (level == PCA9574_HIGH) {
        reg = 0xFF;
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

#define TAG "DUPA"

bool PCA9574_set_level_pin(PCA9574_t *pca, PCA9574_pin_level_t level, uint8_t pin) {
    uint8_t reg = 0x00;
    if (pca->i2c_read_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    if (level == PCA9574_LOW) {
        reg &= ~(1 << pin);
    } else {
        reg |= (1 << pin);
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}