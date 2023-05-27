#ifndef MMC5893_H
#define MMC5983_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define MMC_DEV_ADDR 0x30

#define MMC_REG_X_OUT_0 0x00

#define MMC_REG_STATUS 0x08
#define MMC_STATUS_OTP_READ_DONE (1 << 4)

#define MMC_REG_INTERNAL_CONTROL_0 0x09

#define MMC_REG_INTERNAL_CONTROL_1 0x0A
#define MMC_INTERNAL_CONTORL_1_SW_RST (1 << 7)

#define MMC_REG_INTERNAL_CONTROL_2 0x0B
#define MMC_INTERNAL_CONTROL_2_SET_CM (1 << 3)
#define MMC_INTERNAL_CONTROL_2_SET_EN_PRD (1 << 7)


typedef bool (*mmc5893_i2c_read)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);
typedef bool (*mmc5893_i2c_write)(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, size_t len);
typedef void (*mmc5893_delay)(uint32_t millis);

typedef enum {
    CONTINOUS_MEASUREMENT_OFF = 0x00,
    FREQ_50HZ = 0x04,
    FREQ_100HZ = 0x05
} mmc5983_freq_t;

typedef enum {
    PRD_100 = 0x03,
    PRD_250 = 0x04,
    PRD_500 = 0x05,
    PRD_1000 = 0x06,
} mmc5983_prd_set_t;

typedef struct {
    uint8_t dev_addr;
    mmc5893_i2c_read read_fnc;
    mmc5893_i2c_write write_fnc;
    mmc5893_delay delay_fnc;
} mmc5983_t;

typedef struct {
    float x;
    float y;
    float z;
} mmc5983_mag_t;


bool mmc5893_init(mmc5983_t *dev);
bool mmc5893_auto_SR(mmc5983_t *dev, bool enable);
bool mmc5893_set_continous_mode(mmc5983_t *dev, mmc5983_freq_t freq, mmc5983_prd_set_t prd);
bool mmc5893_mag_ready(mmc5983_t *dev);
bool mmc5893_get_mag_data(mmc5983_t *dev, mmc5983_mag_t *mag);

#endif