#include "gps_task_config.h"
#include "ublox_m8.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "uart.h"
#include "basic_task.h"

static struct {
    ublox_m8_t ubx;
    gps_positioning_t position;
    SemaphoreHandle_t data_mutex;
    basic_task_t task;

    SemaphoreHandle_t processing_mutex;
    gps_task_process_fnc process_fnc;
} gb = {
    .data_mutex = NULL,
    .process_fnc = NULL,
    .processing_mutex = NULL,
};

static void ubx_delay(uint32_t millis) {
    vTaskDelay(pdMS_TO_TICKS(millis));
}

static void process_gps_data(void) {
    ublox_m8_pvt_t pvt;
    ublox_m8_get_PVT(&gb.ubx, &pvt);

    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    gb.position.latitude = pvt.lat.data / 10e6;
    gb.position.longitude = pvt.lon.data / 10e6;
    gb.position.altitude = pvt.height.data / 10e3;
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
    gb.ubx.delay_fnc = ubx_delay;
    gb.ubx.uart_read_fnc = uart_ublox_read;
    gb.ubx.uart_write_fnc = uart_ublox_write;

    if (ublox_m8_init(&gb.ubx) == false) {
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