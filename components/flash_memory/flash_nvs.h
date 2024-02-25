// Copyright 2022 PWr in Space, Krzysztof Gliwiński
// 02.2024 edited by Mateusz Kłosiński
#pragma once

#include <stdint.h>

#include "esp_log.h"
#include "nvs_flash.h"

#define MAX_KEY_LENGTH 14

typedef struct Data {
    char key[MAX_KEY_LENGTH];
    int32_t value;

} NVSData;

typedef enum {
    NVS_OK,
    NVS_INIT_ERROR,
    NVS_OPEN_ERROR,
    NVS_READ_ERROR,
    NVS_NO_MATCHING_KEY,
} NVSResult;

/**
 * @brief
 *
 * @param key
 * @param val
 * @return NVSResult
 */
NVSResult NVS_write_int32(const char* key, int32_t val);
/**
 * @brief
 *
 * @param key
 * @param val
 * @return NVSResult
 */
NVSResult NVS_read_int32t(const char* key, int32_t* val);

/**
 * @brief
 *
 * @param data_array
 * @param length
 * @return NVSResult
 */
NVSResult NVS_init(NVSData* data_array, size_t length);