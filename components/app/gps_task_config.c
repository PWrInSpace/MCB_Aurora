#include "gps_task_config.h"

#include <string.h>

#include "basic_task.h"
#include "errors_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "uart.h"
#include "ublox_m10.h"

static const char *TAG = "GPS";

static struct {
    ublox_m10_t ubx;
    gps_positioning_t position;
    SemaphoreHandle_t data_mutex;
    basic_task_t task;

    SemaphoreHandle_t processing_mutex;
    gps_task_process_fnc process_fnc;
    uint8_t read_error_counter;
} gb = {
    .data_mutex = NULL,
    .process_fnc = NULL,
    .processing_mutex = NULL,
};

static void gps_delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static void process_gps_data(void) {
    ublox_m10_pvt_t pvt;
    if (ublox_m10_get_PVT(&gb.ubx, &pvt) == false) {
        uart_flush_rx();
        uart_flush_tx();
        gb.read_error_counter += 1;
        if (gb.read_error_counter > 7) {
            errors_add(ERROR_TYPE_SENSORS, ERROR_SENSOR_GPS, 100);
        }
        return;
    }

    gb.read_error_counter = 0;

    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    gb.position.latitude = (float)pvt.lat / 1000.0f;
    gb.position.longitude = (float)pvt.lon / 1000.0f;
    gb.position.altitude = (float)pvt.height / 1000.0f;
    gb.position.sats_in_view = pvt.numSV;
    gb.position.fix_type = pvt.fix_type;
    xSemaphoreGive(gb.data_mutex);

    xSemaphoreTake(gb.processing_mutex, portMAX_DELAY);
    if (gb.process_fnc != NULL) {
        gb.process_fnc(&gb.position);
    }
    xSemaphoreGive(gb.processing_mutex);
}

bool gps_change_process_fnc(gps_task_process_fnc fnc) {
    if (xSemaphoreTake(gb.processing_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return false;
    }

    gb.process_fnc = fnc;

    xSemaphoreGive(gb.processing_mutex);

    return true;
}

bool gps_remove_process_fnc(void) {
    if (xSemaphoreTake(gb.processing_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return false;
    }

    gb.process_fnc = NULL;

    xSemaphoreGive(gb.processing_mutex);

    return true;
}

bool initialize_gps(void) {
    gb.ubx.uart_read_fnc = uart_ublox_read;
    gb.ubx.uart_write_fnc = uart_ublox_write;
    gb.ubx.delay_fnc = gps_delay_ms;

    if (ublox_m10_init(&gb.ubx) == false) {
        return false;
    }

    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }

    gb.processing_mutex = xSemaphoreCreateMutex();
    if (gb.processing_mutex == NULL) {
        return false;
    }

    basic_task_cfg_t task_cfg = {
        .process_fnc = process_gps_data,
        .priority = CONFIG_GPS_TASK_PRIORITY,
        .core_id = CONFIG_GPS_TASK_CPU,
        .stack_depth = CONFIG_GPS_TASK_DEPTH,
        .task_delay = CONFIG_GPS_TASK_PERIOD_MS,
    };

    if (basic_task_create(&task_cfg, &gb.task) == false) {
        return false;
    }

    return true;
}

gps_positioning_t gps_get_positioning(void) {
    gps_positioning_t pos;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    pos = gb.position;
    xSemaphoreGive(gb.data_mutex);

    return pos;
}