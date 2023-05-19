#ifndef PCA9574_H
#define PCA9574_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PCA9574_INPUT_PORT_REG 0x00
#define PCA9574_POLARITY_INVERSION_REG 0x01
#define PCA9574_BUS_ENABLE_REG 0x02
#define PCA9574_PULL_SELECTOR_REG 0x03
#define PCA9574_CONFIG_REG 0x04
#define PCA9574_OUTPUT_PORT_REG 0x05
#define PCA9574_INTR_MASK_REG 0x06
#define PCA9574_INTR_STATUS_REG 0x07

typedef enum {
    PCA9574_OUTPUT = 0,
    PCA9574_INPUT,
} PCA9574_pin_mode_t;

typedef enum {
    PCA9574_LOW = 0,
    PCA9574_HIGH,
} PCA9574_pin_level_t;

typedef bool (*PCA9574_i2c_read)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);
typedef bool (*PCA9574_i2c_write)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

typedef struct {
    PCA9574_i2c_read i2c_read_fnc;
    PCA9574_i2c_write i2c_write_fnc;
    uint8_t dev_address;
} PCA9574_config_t;


bool PCA9574_init(PCA9574_config_t *cfg);

bool PCA9574_set_mode(PCA9574_pin_mode_t mode);

bool PCA9574_set_mode_pin(PCA9574_pin_mode_t mode, uint8_t pin);

bool PCA9574_set_level(PCA9574_pin_level_t level);

bool PCA9574_set_level_pin(PCA9574_pin_level_t level, uint8_t pin);

#endif