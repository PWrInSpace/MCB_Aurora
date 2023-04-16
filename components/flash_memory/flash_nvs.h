// Copyright 2022 PWr in Space, Krzysztof Gliwiński
#pragma once

#include <stdint.h>

#include "nvs_flash.h"
#include "esp_log.h"

typedef enum {
  NVS_OK,
  NVS_INIT_ERROR,
  NVS_OPEN_ERROR,
  NVS_READ_ERROR,
} NVSResult;

/**
 * @brief Initialize nvs flash
 *
 * @return NVSResult nvs_status
 */
NVSResult NVS_init(void);

/**
 * @brief Write uint8 to nvs memory
 *
 * @param key key connected with value
 * @param val value
 * @return NVSResult nvs_status
 */
NVSResult NVS_write_uint8(const char* key, uint8_t val);

/**
 * @brief Read uint8 connected to key
 * 
 * @param key key
 * @param val value
 * @return NVSResult nvs_status
 */
NVSResult NVS_read_uint8(const char* key, uint8_t* val);

/**
 * @brief Write uint16 to nvs memory
 *
 * @param key key connected with value
 * @param val value
 * @return NVSResult nvs_status
 */
NVSResult NVS_write_uint16(const char* key, uint16_t val);

/**
 * @brief Read uint16 connected to key
 *
 * @param key key
 * @param val value
 * @return NVSResult nvs_status
 */
NVSResult NVS_read_uint16(const char* key, uint16_t* val);
