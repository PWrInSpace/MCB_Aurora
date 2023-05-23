#include "gps_task_config.h"
#include "ublox_m8.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "uart.h"
#include "gps_task.h"

static struct {
    ublox_m8_t ubx;
    gps_positioning_t position;
    SemaphoreHandle_t data_mutex;
} gb = {
    .data_mutex = NULL,
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
    gb.position.altitude = pvt.height.data / 10e2;
    gb.position.sats_in_view = pvt.numSV;
    xSemaphoreGive(gb.data_mutex);
}

bool gps_initialize(void) {
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

    gps_task_cfg_t task_cfg = {
        .process_fnc = process_gps_data,
    };

    if (gps_create_task(&task_cfg) == false) {
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