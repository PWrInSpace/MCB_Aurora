// Copyright 2022 PWrInSpace, Kuba

#include "esp_now_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"

static struct {
  ENA_device_t *devices;
  size_t number_of_devices;
  ENA_error error_fnc;

  TaskHandle_t api_task;
  QueueHandle_t send_queue;
  uint32_t retakes;
} gb;

static void now_receive_cb(const esp_now_recv_info_t *info, const uint8_t *data, int data_len) {
    ESP_LOGI(TAG, "Packet received, RSSI %d", info->rx_ctrl->rssi);
    if (info->src_addr == NULL || data == NULL || data_len == 0) {
        ESP_LOGE(TAG, "Argument error :C");
        return;
    }

    if (info->rx_ctrl->rx_state != 0) {
        ESP_LOGE(TAG, "Packet error :C");
        return;
    }


}

static void now_send_cb(const uint8_t *mac_addres, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {

    } else {

    }
}

esp_err_t ENA_init(uint8_t *mac_addres) {
  esp_base_mac_addr_set(mac_address);

  // wifi setup
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();

  esp_now_init();

  esp_now_register_recv_cb(now_receive_cb);
  esp_now_register_send_cb(now_send_cb);
}

esp_err_t ENA_register_devices(ENA_device_t *devices, size_t number_of_devices) {
  assert(devices == NULL);
  assert(number_of_devices == 0);

  gb.devices = devices;
  gb.number_of_devices = number_of_devices;

  return ESP_OK;
}

esp_err_t ENA_register_error_handler(ENA_error error_fnc) {
  assert(error_fnc == NULL);

  gb.error_fnc = error_fnc;

  return ESP_OK;
}

// esp_err_t ENA_run(void);
esp_err_t ENA_send(ENA_device_t *device, void *data, size_t data_size);