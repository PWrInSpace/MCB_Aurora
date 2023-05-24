#ifndef PCA9574_H
#define PCA9574_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
} PCA9574_t;

/**
 * @brief Initalize PCA9574
 * 
 * @param pca pointer to pca struct
 * @return true :D
 * @return false :C
 */
bool PCA9574_init(PCA9574_t *pca);

/**
 * @brief Set all pin mode
 * 
 * @param pca pointer to pca struct
 * @param mode gpio pins mode
 * @return true :D
 * @return false :C
 */
bool PCA9574_set_mode(PCA9574_t *pca, PCA9574_pin_mode_t mode);

/**
 * @brief Set pin mode
 * 
 * @param pca pointer to pca struct
 * @param mode pin mode
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool PCA9574_set_mode_pin(PCA9574_t *pca, PCA9574_pin_mode_t mode, uint8_t pin);

/**
 * @brief Set all pin levels
 * 
 * @param pca pointer to pca struct
 * @param level pins level
 * @return true :D
 * @return false :C
 */
bool PCA9574_set_level(PCA9574_t *pca, PCA9574_pin_level_t level);

/**
 * @brief Set pin level
 * 
 * @param pca pointer to pca struct
 * @param level level
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool PCA9574_set_level_pin(PCA9574_t *pca, PCA9574_pin_level_t level, uint8_t pin);

#endif