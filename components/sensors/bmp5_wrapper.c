#include "bmp5_wrapper.h"
#include "bmp5.h"
#include "i2c.h"

#include "esp_log.h"
#define TAG "DUPA"

static struct {
    struct bmp5_dev bmp;
    uint8_t dev_addr;
    struct bmp5_osr_odr_press_config odr_press_cfg;
    float altitude_offset;
} gb;

static BMP5_INTF_RET_TYPE bmp5_i2c_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len,
                                        void *intf_ptr) {
    uint8_t dev_addr = *((uint8_t *)intf_ptr);

    bool result = i2c_sensors_read(dev_addr, reg_addr, read_data, len);
    return result ? BMP5_OK : BMP5_E_COM_FAIL;
}

static BMP5_INTF_RET_TYPE bmp5_i2c_write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len,
                                         void *intf_ptr) {
    uint8_t dev_addr = *((uint8_t *)intf_ptr);

    bool result = i2c_sensors_write(dev_addr, reg_addr, read_data, len);
    return result ? BMP5_OK : BMP5_E_COM_FAIL;
}

static void bmp5_delay(uint32_t period, void *intf_ptr) {
    vTaskDelay(pdMS_TO_TICKS(period));
}

static int8_t bmp5_set_config() {
    int8_t res = 0;

    res = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &gb.bmp);
    if (res != BMP5_OK) {
        return res;
    }

    res = bmp5_get_osr_odr_press_config(&gb.odr_press_cfg, &gb.bmp);
    if (res != BMP5_OK) {
        return res;
    }

    // Enable pressure
    gb.odr_press_cfg.press_en = BMP5_ENABLE;
    gb.odr_press_cfg.odr = BMP5_ODR_120_HZ;
    res = bmp5_set_osr_odr_press_config(&gb.odr_press_cfg, &gb.bmp);
    if (res != BMP5_OK) {
        return res;
    }

    // set temperature and pressure filter
    struct bmp5_iir_config set_iir_cfg;
    set_iir_cfg.set_iir_t = BMP5_IIR_FILTER_COEFF_1;
    set_iir_cfg.set_iir_p = BMP5_IIR_FILTER_COEFF_1;
    set_iir_cfg.shdw_set_iir_t = BMP5_ENABLE;
    set_iir_cfg.shdw_set_iir_p = BMP5_ENABLE;
    res = bmp5_set_iir_config(&set_iir_cfg, &gb.bmp);
    if (res != BMP5_OK) {
        return res;
    }

    res = bmp5_set_power_mode(BMP5_POWERMODE_CONTINOUS, &gb.bmp);
    return res;
}

bool bmp5_wrapper_init(void) {
    gb.dev_addr = BMP5_I2C_ADDR_PRIM;

    gb.bmp.intf = BMP5_I2C_INTF;
    gb.bmp.intf_ptr = &gb.dev_addr;
    gb.bmp.delay_us = bmp5_delay;
    gb.bmp.read = bmp5_i2c_read;
    gb.bmp.write = bmp5_i2c_write;
    gb.altitude_offset = 0;

    // perform soft reset, work around -> reset interrupt not set during normal initialization
    uint8_t data = BMP5_SOFT_RESET_CMD;
    int8_t res = bmp5_set_regs(BMP5_REG_CMD, &data, 1, &gb.bmp);
    if (res != BMP5_OK) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    res = bmp5_init(&gb.bmp);
    if (res != BMP5_OK) {
        return false;
    }

    res = bmp5_set_config();
    return res == BMP5_OK ? true : false;
}

bool bmp5_wrapper_get_data(struct bmp5_sensor_data *data) {
    int8_t res;
    res = bmp5_get_sensor_data(data, &gb.odr_press_cfg, &gb.bmp);

    return res == BMP5_OK ? true : false;
}

bool bmp5_calculate_altitude_offset(void) {
    struct bmp5_sensor_data data;
    float sum = 0;

    // first measurement is skipped, prevent read some strange value after turn on
    if (bmp5_get_sensor_data(&data, &gb.odr_press_cfg, &gb.bmp) != BMP5_OK) {
        return false;
    }

    for (int i = 0; i < BMP5_CALIBRATE_NB_OF_MEAS; ++i) {
        if (bmp5_get_sensor_data(&data, &gb.odr_press_cfg, &gb.bmp) != BMP5_OK) {
            return false;
        }
        sum += bmp5_wrapper_altitude(BMP5_Pa_TO_hPa(data.pressure));
        vTaskDelay(pdTICKS_TO_MS(25));
    }

    gb.altitude_offset = sum / 10.0f;

    return true;
}

float bmp5_wrapper_altitude(float pressure) {
    float altitude = 44330 * (1 - pow((pressure / BMP5_AVERAGE_PRESSURE), 1.f / 5.255f));
    altitude -= gb.altitude_offset;
    return altitude;
}