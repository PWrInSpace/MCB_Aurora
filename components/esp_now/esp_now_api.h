// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_API_H
#define ESP_NOW_API_H

#include "esp_now.h"

#define MAC_ADRESS_SIZE 6
#define MAX_DATA_SIZE 250

typedef enum {
    ENA_OK = 0,
    ENA_REC,
    ENA_SEND,
    ENA_SEND_REPLY,
    ENA_REC_CB_QUEUE,
    ENA_SEND_CB_QUEUE,
} ENA_ERROR;

typedef void(*ENA_on_receive)(uint8_t *data, size_t len);
typedef void(*ENA_on_error)(ENA_ERROR error);

typedef struct {
    uint8_t mac[6];
    // char id;
    ENA_on_receive on_receive;
} ENA_device_t;

typedef struct {
    bool broadcast;
    uint8_t mac[MAC_ADRESS_SIZE];
    uint8_t count;
    uint8_t buffer[MAX_DATA_SIZE];
    size_t len;
} ENA_transmit_param_t;


typedef struct {
    esp_now_send_status_t status;
    uint8_t mac[MAC_ADRESS_SIZE];
} ENA_send_cb_t;

typedef struct {
    uint8_t src_mac[MAC_ADRESS_SIZE];
    uint8_t buffer[MAX_DATA_SIZE];
    size_t len;
} ENA_receive_cb_t;

esp_err_t ENA_init(uint8_t *mac_address);
esp_err_t ENA_register_devices(ENA_device_t *devices, size_t number_of_devices);
esp_err_t ENA_register_error_handler(ENA_on_error error_fnc);
// esp_err_t ENA_run(void);
esp_err_t ENA_send(ENA_device_t *device, void *data, size_t data_size, uint8_t retakes);
// esp_err_t ENA_send_by_id(char id, void *data, size_t data_size);
#endif
