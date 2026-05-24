#include "gps_task_config.h"

#include <string.h>

#include "basic_task.h"
#include "errors_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "uart.h"
#include "ublox_m8.h"

#define TAG "GPS"

static struct {
    ublox_m8_t ubx;
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

static void ubx_delay(uint32_t millis) {
    vTaskDelay(pdMS_TO_TICKS(millis));
}

// --- Konfiguracja GPS ---
static const uint8_t disableNmeaAll[]          = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x9D, 0xDF};
static const uint8_t disableInfMessages[]      = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x92, 0x20, 0x00, 0xB0, 0x63};
static const uint8_t setRocketMode4G[]         = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x21, 0x00, 0x11, 0x20, 0x08, 0x94, 0xB7};
// static const uint8_t enableNavPvt[]            = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x18, 0xE1};
static const uint8_t setRate1Hz_M10[]          = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x21, 0x30, 0xE8, 0x03, 0xF1, 0xAE};
static const char* setUart1OnlyUbxNmea         = "$PUBX,41,1,0003,0001,9600,0*16\r\n";

static void configure_gps_hardware(void) {
    ESP_LOGI(TAG, "Wysyłanie konfiguracji (Rocket Mode, 1Hz, PVT)...");

    // Używamy wskaźnika do funkcji UART, tak jak robi to reszta biblioteki
    gb.ubx.uart_write_fnc((uint8_t*)setUart1OnlyUbxNmea, strlen(setUart1OnlyUbxNmea));
    ubx_delay(50);
    gb.ubx.uart_write_fnc((uint8_t*)disableInfMessages, sizeof(disableInfMessages));
    ubx_delay(50);
    gb.ubx.uart_write_fnc((uint8_t*)disableNmeaAll, sizeof(disableNmeaAll));
    ubx_delay(50);
    gb.ubx.uart_write_fnc((uint8_t*)setRocketMode4G, sizeof(setRocketMode4G));
    ubx_delay(50);
    // gb.ubx.uart_write_fnc((uint8_t*)enableNavPvt, sizeof(enableNavPvt));
    // ubx_delay(50);
    gb.ubx.uart_write_fnc((uint8_t*)setRate1Hz_M10, sizeof(setRate1Hz_M10));
    ubx_delay(100);

    // Czyszczenie bufora RX, żeby pozbyć się starych śmieci/odpowiedzi z GPS
    // (Odpowiednik HAL_UARTEx_ReceiveToIdle_DMA ze STM32)
    uart_flush_rx();
}
// ------------------------

static void process_gps_data(void) {
    ublox_m8_pvt_t pvt;
    if (ublox_m8_get_PVT(&gb.ubx, &pvt) == false) {
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

    // --- WSTRZYKNIĘCIE KONFIGURACJI SPRZĘTOWEJ ---
    configure_gps_hardware();
    // ---------------------------------------------

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