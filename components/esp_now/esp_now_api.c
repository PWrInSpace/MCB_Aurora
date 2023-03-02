// Copyright 2022 PWrInSpace, Kuba

#include <memory.h>

#include "esp_now_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG "ENA"

static struct {
    ENA_device_t *devices;
    size_t number_of_devices;
    ENA_on_error error_fnc;

    ENA_transmit_param_t last_packet;
    bool is_packet_transmiting;

    TaskHandle_t api_task;  // task for handling callbacks
    QueueHandle_t transmit_queue;  // queue for transmiting data
    QueueHandle_t send_cb_queue;  // queue for handling send callback
    QueueHandle_t receive_cb_queue;  // queue for handling receive callback
} gb;

static void now_receive_cb(const uint8_t *mac, const uint8_t *data, int data_len) {
    ESP_LOGI(TAG, "Packet received, RSSI" MACSTR, MAC2STR(mac));
    if (mac == NULL || data == NULL || data_len == 0) {
        ESP_LOGE(TAG, "Argument error :C");
        gb.error_fnc(ENA_REC);
        return;
    }

    ENA_receive_cb_t rec_cb_data;
    memcpy(&rec_cb_data.src_mac, mac, sizeof(MAC_ADRESS_SIZE));
    memcpy(&rec_cb_data.buffer, data, data_len);
    rec_cb_data.len = data_len;

    if (xQueueSend(gb.receive_cb_queue, &rec_cb_data, 0) != pdTRUE) {
        ESP_LOGE(TAG, "Unavle to add receive callback to queue");
        gb.error_fnc(ENA_REC_CB_QUEUE);
    }
}

static void now_send_cb(const uint8_t *mac_addres, esp_now_send_status_t status) {
    ENA_send_cb_t info;

    info.status = status;
    memcpy(&info.mac, mac_addres, MAC_ADRESS_SIZE);

    if (xQueueSend(gb.send_cb_queue, &info, 0) != pdTRUE){
        ESP_LOGE(TAG, "Send callback queeu error");
        gb.error_fnc(ENA_SEND_CB_QUEUE);
    }
}


static bool address_compare(const uint8_t *addr1, const uint8_t *addr2) {
    for (size_t i = 0; i < MAC_ADRESS_SIZE; ++i) {
        if (addr1[i] != addr2[i]) {
            return false;
        }
    }

    return true;
}

static void send_last_packet(void) {
    if (esp_now_send(gb.last_packet.mac, gb.last_packet.buffer, gb.last_packet.len) != ESP_OK) {
        ESP_LOGE(TAG, "Send error");
        gb.error_fnc(ENA_SEND);
    }
}

static void now_task(void *arg) {
    ENA_receive_cb_t rec_cb_data;
    ENA_send_cb_t send_cb_data;

    while (1) {
        if (xQueueReceive(gb.receive_cb_queue, &rec_cb_data, 10) == pdTRUE) {
            for (size_t i = 0; i < gb.number_of_devices; ++i) {
                if (address_compare(gb.devices[i].mac, rec_cb_data.src_mac) == true) {
                    gb.devices[i].on_receive(rec_cb_data.buffer, rec_cb_data.len);
                    break;
                }
            }
        }

        if (xQueueReceive(gb.send_cb_queue, &send_cb_data, 10) == pdTRUE) {
            if (send_cb_data.status == ESP_NOW_SEND_SUCCESS) {
                // there is more data to transmit
                if (xQueueReceive(gb.transmit_queue, &gb.last_packet, 0) == pdTRUE) {
                    send_last_packet();  // clean or not :W
                } else {
                    gb.is_packet_transmiting = false;
                }
            } else {
                if (gb.last_packet.count != 0) {
                    gb.last_packet.count -= 1;
                    send_last_packet();
                } else {
                    gb.error_fnc(ENA_SEND_REPLY);
                    if (xQueueReceive(gb.transmit_queue, &gb.last_packet, 0) == pdTRUE) {
                        send_last_packet();
                    } else {
                        gb.is_packet_transmiting = false;
                    }
                }
            }
        }
    }
}

esp_err_t ENA_init(uint8_t *mac_address) {
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

    gb.transmit_queue = xQueueCreate(5, sizeof(ENA_transmit_param_t));
    gb.send_cb_queue = xQueueCreate(5, sizeof(ENA_send_cb_t));
    gb.receive_cb_queue = xQueueCreate(5, sizeof(ENA_receive_cb_t));

    return ESP_OK;
}

esp_err_t ENA_register_devices(ENA_device_t *devices, size_t number_of_devices) {
  assert(devices == NULL);
  assert(number_of_devices == 0);

  gb.devices = devices;
  gb.number_of_devices = number_of_devices;

  return ESP_OK;
}

esp_err_t ENA_register_error_handler(ENA_on_error error_fnc) {
  assert(error_fnc == NULL);

  gb.error_fnc = error_fnc;

  return ESP_OK;
}

// esp_err_t ENA_run(void);
esp_err_t ENA_send(ENA_device_t *device, void *data, size_t data_size, uint8_t retakes) {
    assert(data == NULL);
    assert(data_size == 0);
    ENA_transmit_param_t send_param;

    if (device == NULL) {
        send_param.broadcast = true;
    } else {
        send_param.broadcast = false;
    }

    memcpy(&send_param.mac, &device->mac, MAC_ADRESS_SIZE);
    memcpy(send_param.buffer, data, data_size);
    send_param.len = data_size;
    send_param.count = retakes;


    if (gb.is_packet_transmiting == true) {
        if (xQueueSend(gb.transmit_queue, &send_param, 10) == pdFALSE) {
            return ESP_FAIL;
        }
    } else {
        gb.last_packet = send_param;
        send_last_packet();
        gb.is_packet_transmiting = true;
    }

    return ESP_OK;
}