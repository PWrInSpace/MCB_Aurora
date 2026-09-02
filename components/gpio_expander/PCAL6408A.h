#ifndef PCA9574_H
#define PCA9574_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PCAL6408A_DEV_ADDRESS 0x20

#define PCAL6408A_INPUT_PORT_REG 0x00
#define PCAL6408A_OUTPUT_PORT_REG 0x01
#define PCAL6408A_POLARITY_INVERSION_REG 0x02
#define PCAL6408A_CONFIGURATION_REG 0x03
#define PCAL6408A_OUTPUT_DRIVE_STRENGTH_0_REG 0x40
#define PCAL6408A_OUTPUT_DRIVE_STRENGTH_1_REG 0x41
#define PCAL6408A_INPUT_LATCH_REG 0x42
#define PCAL6408A_PULLUP_PULLDOWN_ENABLE_REG 0x43
#define PCAL6408A_PULLUP_PULLDOWN_SELECTION_REG 0x44
#define PCAL6408A_INTERRUPT_MASK_REG 0x45
#define PCAL6408A_INTERRUPT_STATUS_REG 0x46
#define PCAL6408A_OUTPUT_PORT_CONFIGURATION_REG 0x4F

typedef enum {
    PCAL6408A_OUTPUT = 0,
    PCAL6408A_INPUT,
} PCAL6408A_pin_mode_t;

typedef enum {
    PCAL6408A_LOW = 0,
    PCAL6408A_HIGH,
} PCAL6408A_pin_level_t;

typedef bool (*PCAL6408A_i2c_read)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);
typedef bool (*PCAL6408A_i2c_write)(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, size_t len);

typedef struct {
    PCAL6408A_i2c_read i2c_read_fnc;
    PCAL6408A_i2c_write i2c_write_fnc;
    uint8_t dev_address;
} PCAL6408A_t;

/**
 * @brief Initalize PCAL6408A
 * 
 * @param pca pointer to pca struct
 * @return true :D
 * @return false :C
 */
bool PCAL6408A_init(PCAL6408A_t *pca);

/**
 * @brief Set all pin mode
 * 
 * @param pca pointer to pca struct
 * @param mode gpio pins mode
 * @return true :D
 * @return false :C
 */
bool PCAL6408A_set_mode(PCAL6408A_t *pca, PCAL6408A_pin_mode_t mode);

/**
 * @brief Set pin mode
 * 
 * @param pca pointer to pca struct
 * @param mode pin mode
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool PCAL6408A_set_mode_pin(PCAL6408A_t *pca, PCAL6408A_pin_mode_t mode, uint8_t pin);

/**
 * @brief Set all pin levels
 * 
 * @param pca pointer to pca struct
 * @param level pins level
 * @return true :D
 * @return false :C
 */
bool PCAL6408A_set_level(PCAL6408A_t *pca, PCAL6408A_pin_level_t level);

/**
 * @brief Set pin level
 * 
 * @param pca pointer to pca struct
 * @param level level
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool PCAL6408A_set_level_pin(PCAL6408A_t *pca, PCAL6408A_pin_level_t level, uint8_t pin);

#endif