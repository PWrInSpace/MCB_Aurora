// Copyright 2022 PWrInSpace, Kuba

#ifndef ESP_NOW_API_H
#define ESP_NOW_API_H

#include "esp_now.h"

typedef void(*ENA_on_receive)(void *data);
typedef void(*ENA_error)(void);

typedef struct {
    char mac[6];
    // char id;
    ENA_on_receive on_receive;
} ENA_device_t;

typedef struct {
    bool broadcast;
    uint8_t *dest_mac;
    uint8_t count;
    uint8_t *buffer;
    size_t len;
} ENA_send_param_t

esp_err_t ENA_init(uint32_t priority, uint32_t depth);
esp_err_t ENA_register_devices(ENA_device_t *devices, size_t number_of_devices);
esp_err_t ENA_register_error_handler(ENA_error error_fnc);
// esp_err_t ENA_run(void);
esp_err_t ENA_send(ENA_device_t *device, void *data, size_t data_size);
// esp_err_t ENA_send_by_id(char id, void *data, size_t data_size);
#endif
