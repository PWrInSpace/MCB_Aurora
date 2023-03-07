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

// TO DO, think about, ENA_send only add to queue, task is sending new package, and queued package,
// will be better for esp_now_send failure

// how sending works
// ENA_send
// -> packet is transmiting -> the packet is added to queue,
// -> packet is not transmiting -> acquire transmiting flag, add data to transmit param, send data
// On_receive
// -> packet transmit succes -> check data on queue (*)
//                              -> more data -> send_data
//                              -> no more data -> relese transmit flag
// -> packet transmit fail -> check retry
//                              -> more retry -> send_data again
//                              -> no more retry ->check data on queue (*)

static struct {
    const ENA_device_t **devices;
    size_t number_of_devices;
    ENA_on_error error_fnc;

    ENA_transmit_param_t last_packet;
    // indicate if packet is transmiting, it also block last packet overwrite
    uint32_t is_packet_transmiting;

    TaskHandle_t api_task;           // task for handling callbacks
    QueueHandle_t transmit_queue;    // queue for transmiting data
    QueueHandle_t send_cb_queue;     // queue for handling send callback
    QueueHandle_t receive_cb_queue;  // queue for handling receive callback
} gb = {.devices = NULL,
        .number_of_devices = 0,
        .error_fnc = NULL,
        .last_packet = {0},
        .is_packet_transmiting = ENA_NOT_SENDING,
        .api_task = NULL,
        .transmit_queue = NULL,
        .send_cb_queue = NULL,
        .receive_cb_queue = NULL};

static void now_receive_cb(const uint8_t *mac, const uint8_t *data, int data_len) {
  ESP_LOGI(TAG, "Packet received, RSSI, MAC: " MACSTR, MAC2STR(mac));
  if (mac == NULL || data == NULL || data_len == 0) {
    ESP_LOGE(TAG, "Argument error :C");
    gb.error_fnc(ENA_REC);
    return;
  }

  ENA_receive_cb_t rec_cb_data;
  memcpy(&rec_cb_data.src_mac, mac, MAC_ADDRESS_SIZE);
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

static void transmiting_acquire(void) {
    if (Atomic_CompareAndSwap_u32(&gb.is_packet_transmiting, ENA_SENDING, ENA_NOT_SENDING) ==
            ATOMIC_COMPARE_AND_SWAP_FAILURE) {
                // debug purpose
                ESP_LOGW(TAG, "Transmiting acquire error, flag val %d", gb.is_packet_transmiting);
    }
}

static void transmiting_release(void) {
    if (Atomic_CompareAndSwap_u32(&gb.is_packet_transmiting, ENA_NOT_SENDING, ENA_SENDING) ==
            ATOMIC_COMPARE_AND_SWAP_FAILURE) {
                // debug purpose
                ESP_LOGW(TAG, "Transmiting release error, flag val %d", gb.is_packet_transmiting);
    }
}

static bool is_packet_transmiting(void) {
    // if is_packet_transmiting == ENA_SENDING, the functions return ...SWAP_SUCCESS,
    // which means that the packet is transmiting ^
    if (Atomic_CompareAndSwap_u32(&gb.is_packet_transmiting, gb.is_packet_transmiting, ENA_SENDING)
            == ATOMIC_COMPARE_AND_SWAP_SUCCESS) {
        return true;
    }
    return false;
}

static bool send_last_packet(void) {
    ESP_LOGI(TAG, "SENDING MESSAGE TO: " MACSTR, MAC2STR(gb.last_packet.mac));
    gb.last_packet.count -= 1;
    if (esp_now_send(gb.last_packet.mac, gb.last_packet.buffer, gb.last_packet.len) != ESP_OK) {
        ESP_LOGE(TAG, "Send error");
        gb.error_fnc(ENA_SEND);
        return false;
    }

    return true;
}

static void transmit_new_packet(void) {
    if (xQueueReceive(gb.transmit_queue, &gb.last_packet, 0) == pdTRUE) {
        if (send_last_packet() == false) {
            transmiting_release();
        }
    } else {
        // if there is no more packet release transmiting flag
        transmiting_release();
    }
}

static void on_receive(ENA_receive_cb_t *rec_cb_data) {
    if (gb.devices == NULL) {
      gb.error_fnc(ENA_DEVICES_NULL);
    }

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

static void on_send(ENA_send_cb_t *send_cb_data) {
    ESP_LOGI(TAG, "SEND CB STATUS %d", send_cb_data->status);

    if (is_packet_transmiting() == false) {
        ESP_LOGE(TAG, "FATAL ERROR, PACKET NOT TRANSMITING");
        transmiting_acquire();
        transmit_new_packet();
    }

    if (send_cb_data->status == ESP_NOW_SEND_SUCCESS) {
        transmit_new_packet();

    } else {
        if (gb.last_packet.count != 0) {
            if (send_last_packet() == false) {
                transmiting_release();
            }
        } else {
            gb.error_fnc(ENA_SEND_REPLY);
            transmit_new_packet();
        }
    }
}

static void now_task(void *arg) {
  ENA_receive_cb_t rec_cb_data;
  ENA_send_cb_t send_cb_data;

  ESP_LOGI(TAG, "Running now task");

    while (1) {
        // on receive
        if (xQueueReceive(gb.receive_cb_queue, &rec_cb_data, 10) == pdTRUE) {
            on_receive(&rec_cb_data);
        }

        // on send
        if (xQueueReceive(gb.send_cb_queue, &send_cb_data, 10) == pdTRUE) {
            on_send(&send_cb_data);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

esp_err_t ENA_init(ENA_config_t *ena_cfg) {
  ESP_RETURN_ON_ERROR(esp_base_mac_addr_set(ena_cfg->mac_address), TAG, "MAC SET");

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

  gb.transmit_queue = xQueueCreate(5, sizeof(ENA_transmit_param_t));
  gb.send_cb_queue = xQueueCreate(5, sizeof(ENA_send_cb_t));
  gb.receive_cb_queue = xQueueCreate(5, sizeof(ENA_receive_cb_t));

  if (gb.transmit_queue == NULL || gb.send_cb_queue == NULL || gb.receive_cb_queue == NULL) {
    return ESP_FAIL;
  }

  xTaskCreatePinnedToCore(now_task, "now_task", ena_cfg->stack_depth, NULL, ena_cfg->priority,
                          &gb.api_task, ena_cfg->core_id);

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

  if (gb.number_of_devices == 0) {
    gb.devices = malloc(sizeof(ENA_device_t *));
    if (gb.devices == NULL) {
      ESP_LOGE(TAG, "PEER ERRROR MEMORY");
      return ESP_ERR_NO_MEM;
    }
    gb.devices[0] = dev;
  } else {
    gb.devices = realloc(gb.devices, sizeof(ENA_device_t *) * (gb.number_of_devices + 1));
    if (gb.devices == NULL) {
      ESP_LOGE(TAG, "PEER ERRROR MEMORY");
      return ESP_ERR_NO_MEM;
    }
    gb.devices[gb.number_of_devices] = dev;
  }

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

    if (is_packet_transmiting() == true) {
        if (xQueueSend(gb.transmit_queue, &send_param, 10) == pdFALSE) {
            return ESP_FAIL;
        }
    } else {
        gb.last_packet = send_param;
        transmiting_acquire();
        if (send_last_packet() == false) {
            transmiting_release();
        }
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
