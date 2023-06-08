// Copyright 2022 PWrInSpace, Kuba

#include "esp_now_api.h"

#include <memory.h>

#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define TAG "ENA"

static struct {
    const ENA_device_t *devices[MAX_DEV_NB];
    size_t number_of_devices;
    ENA_on_error error_fnc;

    bool is_packet_transmiting;

    TaskHandle_t api_task;           // task for handling callbacks
    QueueHandle_t transmit_queue;    // queue for transmiting data
    QueueHandle_t send_cb_queue;     // queue for handling send callback
    QueueHandle_t receive_cb_queue;  // queue for handling receive callback
} gb = {.number_of_devices = 0,
        .error_fnc = NULL,
        .is_packet_transmiting = false,
        .api_task = NULL,
        .transmit_queue = NULL,
        .send_cb_queue = NULL,
        .receive_cb_queue = NULL};

static void now_receive_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    ESP_LOGI(TAG, "Packet received, MAC: " MACSTR, MAC2STR(recv_info->src_addr));
    if (recv_info->src_addr == NULL || data == NULL || data_len == 0) {
        ESP_LOGE(TAG, "Argument error :C");
        gb.error_fnc(ENA_REC);
        return;
    }

    ENA_receive_cb_t rec_cb_data;
    memcpy(&rec_cb_data.src_mac, recv_info->src_addr, MAC_ADDRESS_SIZE);
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
    memcpy(&info.mac, mac_addres, MAC_ADDRESS_SIZE);
    if (xQueueSend(gb.send_cb_queue, &info, 0) != pdTRUE) {
        ESP_LOGE(TAG, "Send callback queeu error");
        gb.error_fnc(ENA_SEND_CB_QUEUE);
    }
}

static bool address_compare(const uint8_t *addr1, const uint8_t *addr2) {
    for (size_t i = 0; i < MAC_ADDRESS_SIZE; ++i) {
        if (addr1[i] != addr2[i]) {
            return false;
        }
    }

    return true;
}

inline static void transmiting_acquire(void) {
    gb.is_packet_transmiting = true;
}

inline static void transmiting_release(void) {
    gb.is_packet_transmiting = false;
}

inline static bool is_packet_transmiting(void) {
    return gb.is_packet_transmiting;
}

static bool send_packet(ENA_transmit_param_t *packet) {
    if (packet->mac[0] != 0xff) {
        ESP_LOGI(TAG, "SENDING MESSAGE TO: " MACSTR, MAC2STR(packet->mac));
    }

    if (packet->count != 0) {
        packet->count -= 1;
    }

    if (esp_now_send(packet->mac, packet->buffer, packet->len) != ESP_OK) {
        ESP_LOGE(TAG, "Send error");
        gb.error_fnc(ENA_SEND);
        return false;
    }

    return true;
}

static void on_receive(ENA_receive_cb_t *rec_cb_data) {
    for (size_t i = 0; i < gb.number_of_devices; ++i) {
        if (address_compare(gb.devices[i]->peer.peer_addr, rec_cb_data->src_mac) == true) {
            if (gb.devices[i]->on_receive != NULL) {
                gb.devices[i]->on_receive(rec_cb_data->buffer, rec_cb_data->len);
            }
            break;
        } else if (i == (gb.number_of_devices - 1)) {
            gb.error_fnc(ENA_UNKNOWN_DEV);
        }
    }
}

static void on_send(ENA_send_cb_t *send_cb_data, ENA_transmit_param_t *packet) {
    ESP_LOGD(TAG, "SEND CB, STATUS %d", send_cb_data->status);

    if (is_packet_transmiting() == false) {
        ESP_LOGE(TAG, "FATAL ERROR, PACKET NOT TRANSMITING");
        return;
    }

    if (send_cb_data->status == ESP_NOW_SEND_SUCCESS) {
        transmiting_release();  // done, packet send
    } else {
        if (packet->count != 0) {
            if (send_packet(packet) == false) {
                transmiting_release();
            }
        } else {
            gb.error_fnc(ENA_SEND_REPLY);
            transmiting_release();
        }
    }
}

static void transmit_package(ENA_transmit_param_t *packet) {
    transmiting_acquire();
    if (send_packet(packet) == false) {
        transmiting_release();
    }
}

static void now_task(void *arg) {
    ENA_receive_cb_t rec_cb_data;
    ENA_send_cb_t send_cb_data;
    ENA_transmit_param_t last_packet;

    ESP_LOGI(TAG, "Running now task");

    while (1) {
        // on receive
        if (xQueueReceive(gb.receive_cb_queue, &rec_cb_data, 10) == pdTRUE) {
            on_receive(&rec_cb_data);
        }

        // on send
        if (xQueueReceive(gb.send_cb_queue, &send_cb_data, 10) == pdTRUE) {
            on_send(&send_cb_data, &last_packet);
        }

        if (is_packet_transmiting() == false) {
            if (xQueueReceive(gb.transmit_queue, &last_packet, 10) == pdTRUE) {
                transmit_package(&last_packet);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

esp_err_t ENA_init(uint8_t *mac_address) {
    ESP_RETURN_ON_ERROR(esp_base_mac_addr_set(mac_address), TAG, "MAC SET");

    uint8_t mac[6] = {0};
    esp_base_mac_addr_get(mac);
    ESP_LOGI(TAG, "MAC ADDRESS" MACSTR, MAC2STR(mac));

    // wifi setup
    ESP_RETURN_ON_ERROR(nvs_flash_init(), TAG, "NVS FLASH");
    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "NETIF");
    ESP_RETURN_ON_ERROR(esp_event_loop_create_default(), TAG, "EVENT LOOP");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "WIFI INIT");
    ESP_RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), TAG, "WIFI STORAGE");
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "WIFI MODE");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "WIFI START");

    ESP_RETURN_ON_ERROR(esp_now_init(), TAG, "ESP NOW INIT");

    ESP_RETURN_ON_ERROR(esp_now_register_recv_cb(now_receive_cb), TAG, "REC CB");
    ESP_RETURN_ON_ERROR(esp_now_register_send_cb(now_send_cb), TAG, "SEND CB");

    return ESP_OK;
}

esp_err_t ENA_run(ENA_config_t *cfg) {
    gb.transmit_queue = xQueueCreate(5, sizeof(ENA_transmit_param_t));
    gb.send_cb_queue = xQueueCreate(5, sizeof(ENA_send_cb_t));
    gb.receive_cb_queue = xQueueCreate(5, sizeof(ENA_receive_cb_t));

    if (gb.transmit_queue == NULL || gb.send_cb_queue == NULL || gb.receive_cb_queue == NULL) {
        return ESP_FAIL;
    }

    xTaskCreatePinnedToCore(now_task, "now_task", cfg->stack_depth, NULL, cfg->priority,
                            &gb.api_task, cfg->core_id);

    if (gb.api_task == NULL) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t ENA_register_device(const ENA_device_t *dev) {
    assert(dev != NULL);
    if (dev == NULL) {
        return ESP_FAIL;
    }

    if (esp_now_add_peer(&dev->peer) != ESP_OK) {
        ESP_LOGE(TAG, "ADD PEER ERROR, MAC: " MACSTR, MAC2STR(dev->peer.peer_addr));
        return ESP_FAIL;
    }

    if ((gb.number_of_devices + 1) >= MAX_DEV_NB) {
        ESP_LOGE(TAG, "Max number of devices reached, you can extend number in menuconfig");
        return ESP_FAIL;
    }

    gb.devices[gb.number_of_devices] = dev;

    gb.number_of_devices += 1;
    return ESP_OK;
}

esp_err_t ENA_register_error_handler(ENA_on_error error_fnc) {
    assert(error_fnc != NULL);
    if (error_fnc == NULL) {
        return ESP_FAIL;
    }

    gb.error_fnc = error_fnc;

    return ESP_OK;
}

esp_err_t ENA_send(const ENA_device_t *device, uint8_t *data, size_t data_size, uint8_t retakes) {
    assert(data != NULL);
    assert(data_size != 0);
    if (data == NULL || data_size == 0) {
        return ESP_FAIL;
    }

    ENA_transmit_param_t send_param;
    send_param.mac = device->peer.peer_addr;
    memcpy(send_param.buffer, data, data_size);
    send_param.len = data_size;
    send_param.count = retakes;

    if (xQueueSend(gb.transmit_queue, &send_param, 10) == pdFALSE) {
        ESP_LOGE(TAG, "UNABLE TO ADD DATA TO TRANSMIT QUEUE");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t ENA_get_dev_info(ENA_device_t *dev, size_t nb) {
    if (nb > gb.number_of_devices) {
        return ESP_FAIL;
    }

    memcpy(dev, gb.devices[nb], MAC_ADDRESS_SIZE);
    return ESP_OK;
}
