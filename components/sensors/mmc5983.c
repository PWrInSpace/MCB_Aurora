#include "mmc5983.h"

static bool i2c_write(mmc5983_t *dev, uint8_t reg, const uint8_t *data, size_t len) {
    if (dev->write_fnc == NULL) {
        return false;
    }

    if (dev->write_fnc(dev->dev_addr, reg, data, len) == false) {
        return false;
    }

    return true;
}

static bool i2c_read(mmc5983_t *dev, uint8_t reg,  uint8_t *data, size_t len) {
    if (dev->read_fnc == NULL) {
        return false;
    }

    if (dev->read_fnc(dev->dev_addr, reg, data, len) == false) {
        return false;
    }

    return true;
}

bool mmc5893_init(mmc5983_t *dev) {
    if (dev->read_fnc == NULL || dev->write_fnc == NULL || dev->delay_fnc == NULL) {
        return false;
    }

    if (dev->dev_addr != MMC_DEV_ADDR) {
        return false;
    }

    uint8_t data = MMC_INTERNAL_CONTORL_1_SW_RST;
    if (i2c_write(dev, MMC_REG_INTERNAL_CONTROL_1, &data, 1) == false) {
        return false;
    }

    dev->delay_fnc(15);

    uint8_t status;
    if (i2c_read(dev, MMC_REG_STATUS, &status, 1) == false) {
        return false;
    }

    if (!(status & MMC_STATUS_OTP_READ_DONE)) {
        return false;
    }

    return true;
}

bool mmc5893_auto_SR(mmc5983_t *dev, bool enable) {
    uint8_t internal_control_0;

    if (i2c_read(dev, MMC_REG_INTERNAL_CONTROL_0, &internal_control_0, 1) == false) {
        return false;
    }

    internal_control_0 |= (enable << 5);
    if (i2c_write(dev, MMC_REG_INTERNAL_CONTROL_0, &internal_control_0, 1) == false) {
        return false;
    }

    return true;
}

bool mmc5893_set_continous_mode(mmc5983_t *dev, mmc5983_freq_t freq, mmc5983_prd_set_t prd) {
    if (mmc5893_auto_SR(dev, true) == false) {
        return false;
    }

    uint8_t internal_control_2 = 0;
    internal_control_2 |= freq;
    internal_control_2 |= MMC_INTERNAL_CONTROL_2_SET_CM;
    internal_control_2 |= (prd << 4);
    internal_control_2 |= MMC_INTERNAL_CONTROL_2_SET_EN_PRD;

    if (i2c_write(dev, MMC_REG_INTERNAL_CONTROL_2, &internal_control_2, 1) == false) {
        return false;
    }

    return true;
}

bool mmc5893_mag_ready(mmc5983_t *dev) {
    uint8_t status;
    if (i2c_read(dev, MMC_REG_STATUS, &status, 1) == false) {
        return false;
    }

    return (status & 0x01);
}

bool mmc5893_get_mag_data(mmc5983_t *dev, mmc5983_mag_t *mag) {
    uint8_t data[6];
    if (i2c_read(dev, MMC_REG_X_OUT_0, data, sizeof(data)) == false) {
        return false;
    }

    uint16_t value = 0;
    value |= data[1];
    value |= (data[0] << 8);
    mag->x = (float)(value - 32768.0);
    mag->x = mag->x / 4096.0;

    value = 0;
    value |= data[3];
    value |= (data[2] << 8);
    mag->y = (float)(value - 32768.0);
    mag->y = mag->y / 4096.0;

    value = 0;
    value |= data[5];
    value |= (data[4] << 8);
    mag->z = (float)(value - 32768.0);
    mag->z = mag->z / 4096.0;

    return true;
}


