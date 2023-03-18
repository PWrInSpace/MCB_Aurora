// Copyright 2022 PWrInSpace, Kuba

#include "esp_now_config.h"
#include "esp_log.h"
#include "esp_now_api.h"

#define TAG "ENC"

static void callback_pitot(uint8_t *data, size_t size);
static void callback_vent_valve(uint8_t *data, size_t size);
static void callback_main_valve(uint8_t *data, size_t size);
static void callback_tanwa(uint8_t *data, size_t size);

const ENA_device_t esp_now_broadcast = {
    .peer = {.peer_addr = BROADCAST_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = NULL,
};

const ENA_device_t esp_now_pitot = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_pitot,
};

const ENA_device_t esp_now_vent_valve = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_vent_valve,
};

const ENA_device_t esp_now_main_valve = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_main_valve,
};

const ENA_device_t esp_now_tanwa = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_tanwa,
};

// static void on_err(ENA_ERROR err) {
//     ESP_LOGE(TAG, "ERROR :C %d", err);
// }

static void callback_pitot(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Pitot receive, size %d", size);
}

static void callback_vent_valve(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Vent receive, size %d", size);
}

static void callback_main_valve(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Main receive, size %d", size);
}

static void callback_tanwa(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Tanwa receive, size %d", size);
}
