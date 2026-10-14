#include "PCAL6408A.h"

#include <assert.h>

#include "esp_log.h"
#include "gpio_expander.h"

static const char *TAG = "PCAL6408A";

bool PCAL6408A_init(PCAL6408A_t *pca) {
    if (pca->i2c_read_fnc == NULL || pca->i2c_write_fnc == NULL) {
        ESP_LOGE(TAG, "I2C read/write function pointers are NULL");
        return false;
    }

    uint8_t input_port = 0;
    if (pca->i2c_read_fnc(pca->dev_address, PCAL6408A_INPUT_PORT_REG, &input_port, 1) == false) {
        return false;
    }

    return true;
}

bool PCAL6408A_get_reg(PCAL6408A_t *pca, uint8_t reg_addr, uint8_t *reg_value) {
    if (pca->i2c_read_fnc(pca->dev_address, reg_addr, reg_value, 1) == false) {
        return false;
    }
    return true;
}

bool PCAL6408A_set_mode(PCAL6408A_t *pca, PCAL6408A_pin_mode_t mode) {
    uint8_t reg;

    switch (mode) {
        case PCAL6408A_OUTPUT:
            reg = 0x00;
            break;
        case PCAL6408A_INPUT:
            reg = 0xFF;
            break;
        default:
            ESP_LOGE(TAG, "Invalid mode");
            return false;
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_CONFIGURATION_REG, &reg, 1) == false) {
        return false;
    }

    // We have to set all pins to low because in output mode their default state is high
    if (mode == PCAL6408A_OUTPUT) {
        reg = 0x00;
        if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_OUTPUT_PORT_REG, &reg, 1) == false) {
            return false;
        }
    }

    return true;
}

bool PCAL6408A_set_mode_pin(PCAL6408A_t *pca, PCAL6408A_pin_mode_t mode, uint8_t pin) {
    if (7 < pin) {
        ESP_LOGE(TAG, "Invalid pin number");
        return false;
    }

    uint8_t reg = 0x00;
    if (PCAL6408A_get_reg(pca, PCAL6408A_CONFIGURATION_REG, &reg) == false) {
        return false;
    }

    switch (mode) {
        case PCAL6408A_OUTPUT:
            reg &= ~(1 << pin);
            break;
        case PCAL6408A_INPUT:
            reg |= 1 << pin;
            break;
        default:
            ESP_LOGE(TAG, "Invalid mode");
            return false;
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_CONFIGURATION_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCAL6408A_set_level(PCAL6408A_t *pca, PCAL6408A_pin_level_t level) {
    uint8_t reg;

    switch (level) {
        case PCAL6408A_LOW:
            reg = 0x00;
            break;
        case PCAL6408A_HIGH:
            reg = 0xFF;
            break;
        default:
            ESP_LOGE(TAG, "Invalid level");
            return false;
    }

    if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCAL6408A_set_level_pin(PCAL6408A_t *pca, PCAL6408A_pin_level_t level, uint8_t pin) {
    if (7 < pin) {
        ESP_LOGE(TAG, "Invalid pin number");
        return false;
    }

    uint8_t reg = 0x00;
    if (PCAL6408A_get_reg(pca, PCAL6408A_OUTPUT_PORT_REG, &reg) == false) {
        return false;
    }

    switch (level) {
        case PCAL6408A_LOW:
            reg &= ~(1 << pin);
            break;
        case PCAL6408A_HIGH:
            reg |= 1 << pin;
            break;
        default:
            ESP_LOGE(TAG, "Invalid mode");
            return false;
    }

    if (pin != EXPANDER_LORA_RESET_PIN && (reg & (1 << EXPANDER_LORA_RESET_PIN)) == 0) {
        ESP_LOGW(TAG, "Prevented accidental LoRa reset assertion");
        reg |= 1 << EXPANDER_LORA_RESET_PIN;
    }
    if (pin != EXPANDER_GPS_RESET_PIN && (reg & (1 << EXPANDER_GPS_RESET_PIN)) == 0) {
        reg |= 1 << EXPANDER_GPS_RESET_PIN;
    }

    ESP_LOGI(TAG, "Setting reg: 0x%02X for pin %d to level %d", reg, pin, level);
    if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool PCAL6408A_set_diode_color(PCAL6408A_t *pca, gpio_exp_led_colors_t color) {
    uint8_t reg = 0x00;
    if (PCAL6408A_get_reg(pca, PCAL6408A_OUTPUT_PORT_REG, &reg) == false) {
        return false;
    }

    reg &= ~(111 << EXPANDER_LED_PIN_RED);
    reg |= (color & 0x01) << EXPANDER_LED_PIN_RED;
    reg |= ((color >> 1) & 0x01) << EXPANDER_LED_PIN_GREEN;
    reg |= ((color >> 2) & 0x01) << EXPANDER_LED_PIN_BLUE;

    if (pca->i2c_write_fnc(pca->dev_address, PCAL6408A_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }
    return true;
}