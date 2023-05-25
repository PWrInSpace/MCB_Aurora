
#include <math.h>
#include "bmi08.h"
#include "bmi088_mm.h"
#include "i2c.h"


#define GRAVITY_EARTH  (9.80665f)

static struct {
    struct bmi08_dev dev;
    uint8_t acc_addr;
    uint8_t gyro_addr;
} gb;

static BMI08_INTF_RET_TYPE bmi08_i2c_read(uint8_t reg_addr, uint8_t *read_data, uint32_t len,
                                        void *intf_ptr) {
    uint8_t dev_addr = *((uint8_t *)intf_ptr);

    bool result = i2c_sensors_read(dev_addr, reg_addr, read_data, len);
    return result ? BMI08_OK : BMI08_E_COM_FAIL;
}

static BMI08_INTF_RET_TYPE bmi08_i2c_write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len,
                                         void *intf_ptr) {
    uint8_t dev_addr = *((uint8_t *)intf_ptr);

    bool result = i2c_sensors_write(dev_addr, reg_addr, read_data, len);
    return result ? BMI08_OK : BMI08_E_COM_FAIL;
}

static void bmi08_delay(uint32_t period, void *intf_ptr) {
    vTaskDelay(pdMS_TO_TICKS(period / 1000));
}

static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    double power = 2;

    float half_scale = (float)((pow((double)power, (double)bit_width) / 2.0f));

    return (dps / (half_scale)) * (val);
}

bool bmi08_wrapper_init(void) {
    gb.acc_addr = BMI08_ACCEL_I2C_ADDR_PRIMARY;
    gb.gyro_addr = BMI08_GYRO_I2C_ADDR_PRIMARY;

    gb.dev.write = bmi08_i2c_write;
    gb.dev.read = bmi08_i2c_read;
    gb.dev.delay_us = bmi08_delay;

    gb.dev.intf = BMI08_I2C_INTF;
    gb.dev.intf_ptr_accel = &gb.acc_addr;
    gb.dev.intf_ptr_gyro = &gb.gyro_addr;
    gb.dev.read_write_len = 32;

    if (bmi088_mma_init(&gb.dev) != BMI08_OK || bmi08g_init(&gb.dev) != BMI08_OK) {
        return false;
    }

    /* Reset the accelerometer */
    if(bmi08a_soft_reset(&gb.dev) != BMI08_OK) {
        return false;
    }

    gb.dev.accel_cfg.power = BMI08_ACCEL_PM_ACTIVE;
    if (bmi08a_set_power_mode(&gb.dev) != BMI08_OK) {
        return false;
    }

    gb.dev.gyro_cfg.power = BMI08_GYRO_PM_NORMAL;
    if (bmi08g_set_power_mode(&gb.dev) != BMI08_OK) {
        return false;
    }

    if (bmi08a_load_config_file(&gb.dev) != BMI08_OK) {
        return false;
    }


    gb.dev.accel_cfg.range = BMI088_MM_ACCEL_RANGE_24G;
    gb.dev.accel_cfg.odr = BMI08_ACCEL_ODR_100_HZ;
    gb.dev.accel_cfg.bw = BMI08_ACCEL_BW_NORMAL;
    gb.dev.gyro_cfg.range = BMI08_GYRO_RANGE_2000_DPS;
    gb.dev.gyro_cfg.odr = BMI08_GYRO_BW_32_ODR_100_HZ;
    gb.dev.gyro_cfg.bw = BMI08_GYRO_BW_32_ODR_100_HZ;
    gb.dev.gyro_cfg.power = BMI08_GYRO_PM_NORMAL;
    if (bmi088_mma_set_meas_conf(&gb.dev) != BMI08_OK) {
        return false;
    }

    return true;
}

// bool bmp5_wrapper_get_data(struct bmp5_sensor_data *data) {
//     int8_t res;
//     res = bmp5_get_sensor_data(data, &gb.odr_press_cfg, &gb.bmp);

//     return res == BMP5_OK ? true : false;
// }

bool bmi08_acc_data_ready(void) {
    uint8_t status;
    if (bmi08a_get_data_int_status(&status, &gb.dev) != BMI08_OK) {
        return false;
    }


    return (status & BMI08_ACCEL_DATA_READY_INT);
}

bool bmi08_gyro_data_ready(void) {
    uint8_t status;
    if (bmi08g_get_data_int_status(&status, &gb.dev) != BMI08_OK) {
        return false;
    }


    return (status & BMI08_GYRO_DATA_READY_INT);
}

bool bmi08_get_acc_data(struct bmi08_sensor_data_f *acc) {
    struct bmi08_sensor_data data;
    if (bmi088_mma_get_data(&data, &gb.dev) != BMI08_OK) {
        return false;
    }

    acc->x = lsb_to_mps2(data.x, (float)24, 16);
    acc->y = lsb_to_mps2(data.y, (float)24, 16);
    acc->z = lsb_to_mps2(data.z, (float)24, 16);

    return true;
}

bool bmi08_get_gyro_data(struct bmi08_sensor_data_f *gyro) {
    struct bmi08_sensor_data data;
    if (bmi08g_get_data(&data, &gb.dev) != BMI08_OK) {
        return false;
    }

    gyro->x = lsb_to_dps(data.x, (float)2000, 16);
    gyro->y = lsb_to_dps(data.y, (float)2000, 16);
    gyro->z = lsb_to_dps(data.z, (float)2000, 16);

    return true;
}