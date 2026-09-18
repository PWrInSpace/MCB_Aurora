// Copyright 2022 PWrInSpace
#include "processing_task_config.h"

#include <math.h>

#include "bmi08_wrapper.h"
#include "bmp5_wrapper.h"
#include "errors_config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "fusion_wrapper.h"
#include "mag_wrapper.h"
#include "physics.h"
#include "state_machine.h"
#include "kalman.h"

static const char *TAG = "SENSORS_CFG";

static sensors_data_t sensors_data;
static struct bmi08_sensor_data_f acc;
static struct bmi08_sensor_data_f gyro;
static struct bmp5_sensor_data baro;
static FusionAhrs ahrs;
static kalman_t kf;
static mmc5983_mag_t mag;

static void calculate_base_pressure() {
    float pressure_sum = 0.0f;
    for (int i = 0; i < BMP5_CALIBRATE_NB_OF_MEAS; i++) {
        struct bmp5_sensor_data data;
        if (bmp5_get_data(&data)) {
            pressure_sum += data.pressure;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    sensors_data.pressure0 = pressure_sum / BMP5_CALIBRATE_NB_OF_MEAS;
}

static void sensors_read_data(void *data_buffer) {
    sensors_data_t *data = data_buffer;

    if (mag_data_ready() == true && mag_get_data(&mag) == false) {
        errors_add(ERROR_TYPE_SENSORS, ERROR_SENSOR_MAG, 100);
    }
    data->mag_x = mag.x;
    data->mag_y = mag.y;
    data->mag_z = mag.z;

    if (bmi08_get_acc_data(&acc) == false) {
        errors_add(ERROR_TYPE_SENSORS, ERROR_SENSOR_IMU, 100);
    }
    data->acc_x = acc.x;
    data->acc_y = acc.y;
    data->acc_z = acc.z;

    if (bmi08_get_gyro_data(&gyro) == false) {
        errors_add(ERROR_TYPE_SENSORS, ERROR_SENSOR_IMU, 100);
    }
    data->gyr_x = gyro.x;
    data->gyr_y = gyro.y;
    data->gyr_z = gyro.z;

    if (bmp5_get_data(&baro) == false) {
        errors_add(ERROR_TYPE_SENSORS, ERROR_SENSOR_BAR, 100);
    }
    data->pressure = BMP5_Pa_TO_hPa(baro.pressure);
    data->temperature = baro.temperature;

    uint64_t current_time = esp_timer_get_time();
    float dt = 0.2f;
    if (data->timestamp != 0) {
        dt = (float)(current_time - data->timestamp) / 1000000.0f;
    }
    data->dt = dt;
    data->timestamp = current_time;
}

static void sensors_process_data(void *data_buffer) {
    sensors_data_t *data = data_buffer;

    data->altitude = hypsometric_altitude(data->pressure, data->pressure0, data->temperature);

    fusion_wrapper_update(&ahrs, data->gyr_x, data->gyr_y, data->gyr_z, data->acc_x, data->acc_y,
                          data->acc_z, data->mag_x, data->mag_y, data->mag_z, data->dt);
    fusion_data_t fusion_data = fusion_wrapper_get_data(&ahrs);

    data->acc_x = fusion_data.acceleration.axis.x;
    data->acc_y = fusion_data.acceleration.axis.y;
    data->acc_z = fusion_data.acceleration.axis.z;
    data->acc_vertical = fusion_data.acceleration_earth.axis.z;

    kalman_predict(&kf, data->acc_vertical, data->dt);
    kalman_update(&kf, data->altitude);

    data->altitude = kf.alt;
    data->velocity = kf.vel;
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

    if (mag_init() == false) {
        ESP_LOGE(TAG, "MAG");
        return false;
    }

    if (mag_set_continous_mode(FREQ_100HZ, PRD_500) == false) {
        ESP_LOGE(TAG, "BMAGMODE");
        return false;
    }

    if (fusion_wrapper_init(&ahrs) == false) {
        ESP_LOGE(TAG, "FUSION");
        return false;
    }

    if (kalman_init(&kf, 0.1f, 0.5f) == false) {
        ESP_LOGE(TAG, "KALMAN");
        return false;
    }

    sensors_task_cfg_t cfg = {.sensors_read_fnc = sensors_read_data,
                              .sensors_process_fnc = sensors_process_data,
                              .data_size = sizeof(sensors_data_t),
                              .data_buffer = &sensors_data};
    calculate_base_pressure();

    return sensors_create_task(&cfg);
}