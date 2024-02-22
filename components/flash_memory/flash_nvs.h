// Copyright 2022 PWr in Space, Krzysztof Gliwiński
// 02.2024 edited by Mateusz Kłosiński
#pragma once

#include <stdint.h>

#include "esp_log.h"
#include "nvs_flash.h"

#define MAX_KEY_LENGTH 14

typedef struct Data {
    char key[MAX_KEY_LENGTH];
    uint32_t value;

} NVSData;

typedef enum {
    NVS_OK,
    NVS_INIT_ERROR,
    NVS_OPEN_ERROR,
    NVS_READ_ERROR,
} NVSResult;

/**
 * @brief Construct a new nvs write struct array object
 *
 * @param array
 * @param array_length
 */

/**
 * @brief
 *
 * @param struct_data
 * @return NVSResult
 */

NVSResult NVS_write_uint32(const char* key, uint32_t val);

NVSResult NVS_read_uint32t(const char* key, uint32_t* val);

NVSResult NVS_write_data(NVSData struct_data);
/**
 * @brief Initialize nvs flash
 *
 * @return NVSResult nvs_status
 */
NVSResult NVS_init(NVSData* data_array, uint8_t length);

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