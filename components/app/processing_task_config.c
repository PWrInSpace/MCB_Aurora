// Copyright 2022 PWrInSpace
#include "processing_task_config.h"

#include "bmi08_wrapper.h"
#include "bmp5_wrapper.h"
#include "errors_config.h"
#include "esp_log.h"
#include "mag_wrapper.h"
#include "magdwick.h"
#include "rocket_data.h"

#define TAG "SENSORS_CFG"

static struct bmi08_sensor_data_f acc;
static struct bmi08_sensor_data_f gyro;
static mmc5983_mag_t mag;
static struct bmp5_sensor_data bar;
static struct mgos_imu_madgwick madgwick;

static void sensors_read_data(void *data_buffer) {
    sensors_data_t *data = (sensors_data_t *)data_buffer;

    // if (mag_data_ready() == true) {
    //     if (mag_get_data(&mag) == false) {
    //         // error
    //     }
    // }

    if (bmi08_get_acc_data(&acc) == false) {
        // error
    }

    if (bmi08_get_gyro_data(&gyro) == false) {
        // error
    }

    if (bmp5_wrapper_get_data(&bar) == false) {
        // error
    }

    mgos_imu_madgwick_update(&madgwick, gyro.x, gyro.y, gyro.z, acc.x, acc.y, acc.z, mag.x, mag.y,
                             mag.z);

    data->acc_x = acc.x;
    data->acc_y = acc.y;
    data->acc_z = acc.z;

    data->gyr_x = gyro.x;
    data->gyr_x = gyro.x;
    data->gyr_x = gyro.x;

    data->mag_x = mag.x;
    data->mag_y = mag.y;
    data->mag_z = mag.z;

    data->pressure = bar.pressure / 100.0;
    data->temperature = bar.temperature;

    mgos_imu_madgwick_get_angles(&madgwick, &data->roll, &data->pitch, &data->yaw);
}


bool initialize_processing_task(void) {
    if (bmi08_wrapper_init() == false) {
        ESP_LOGE(TAG, "BMI08");
        return false;
    }

    if (bmp5_wrapper_init() == false) {
        ESP_LOGE(TAG, "BMP5");
        return false;
    }

    // if (mag_init() == false) {
    //     ESP_LOGE(TAG, "MAG");
    //     return false;
    // }

    // if (mag_set_continous_mode(FREQ_100HZ, PRD_500) == false) {
    //     ESP_LOGE(TAG, "BMAGMODE");
    //     return false;
    // }

    if (mgos_imu_madgwick_create(&madgwick) == false) {
        ESP_LOGE(TAG, "MADGWICK");
        return false;
    }

    if (mgos_imu_madgwick_set_params(&madgwick, 100, 0.01) == false) {
        ESP_LOGE(TAG, "MADGWICK 2");
        return false;
    }

    mgos_imu_madgwick_reset(&madgwick);

        sensors_task_cfg_t cfg = {
            .sensors_read_fnc = sensors_read_data,
            .sensors_process_fnc = NULL,
            .data_size = sizeof(sensors_data_t),
        };

    return sensors_create_task(&cfg);
}