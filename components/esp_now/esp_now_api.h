// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_API_H
#define ESP_NOW_API_H

#include "esp_now.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/atomic.h"

#define MAC_ADDRESS_SIZE 6
#define MAX_DATA_SIZE 250

typedef enum {
    ENA_NOT_SENDING,
    ENA_SENDING,
} ENA_TRANSMITING_STATUS;

typedef enum {
  ENA_OK = 0,
  ENA_REC,
  ENA_SEND,
  ENA_SEND_REPLY,
  ENA_REC_CB_QUEUE,
  ENA_SEND_CB_QUEUE,
  ENA_DEVICES_NULL,
  ENA_UNKNOWN_DEV,
} ENA_ERROR;

typedef void (*ENA_on_receive)(uint8_t *data, size_t len);
typedef void (*ENA_on_error)(ENA_ERROR error);

typedef struct {
  esp_now_peer_info_t peer;
  ENA_on_receive on_receive;
} ENA_device_t;

typedef struct {
  uint8_t mac_address[MAC_ADDRESS_SIZE];
  uint32_t stack_depth;
  BaseType_t core_id;
  UBaseType_t priority;
} ENA_config_t;

typedef struct {
  uint8_t *mac;
  uint8_t count;
  uint8_t buffer[MAX_DATA_SIZE];  // malloc???
  size_t len;
} ENA_transmit_param_t;

typedef struct {
  esp_now_send_status_t status;
  uint8_t mac[MAC_ADDRESS_SIZE];
} ENA_send_cb_t;

typedef struct {
  uint8_t src_mac[MAC_ADDRESS_SIZE];
  uint8_t buffer[MAX_DATA_SIZE];  // malloc???
  size_t len;
} ENA_receive_cb_t;

esp_err_t ENA_init(ENA_config_t *ena_cfg);
esp_err_t ENA_register_device(ENA_device_t *device);
esp_err_t ENA_register_error_handler(ENA_on_error error_fnc);
esp_err_t ENA_send(ENA_device_t *device, void *data, size_t data_size, uint8_t retakes);

/**
 * @brief Function for unit testing, return saved device info
 *
 * @param dev pointer to dev struct
 * @param nb number of device
 * @return esp_err_t status
 */
esp_err_t ENA_get_dev_info(ENA_device_t *dev, size_t nb);

#endif
