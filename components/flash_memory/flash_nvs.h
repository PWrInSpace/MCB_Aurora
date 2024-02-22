// Copyright 2022 PWr in Space, Krzysztof Gliwiński
//02.2024 edited by Mateusz Kłosiński
#pragma once

#include <stdint.h>

#include "nvs_flash.h"
#include "esp_log.h"

/**
 * #include "biblioteka_nvs.h"

#define KEY_1 "1"
#define KEY_1_VALUE 1
#define KEY_2 "2"
#define KEY_2_VALUE 2
#define KEY_3 "3"
#define KEY_3_VALUE 3

void app_main(void) {
    ESP_LOGI(TAG, "INIT TASK");

    nvs_mem_t key_val_pairs[] = {
        {KEY_1, KEY_1_VALUE},
        {KEY_2, KEY_2_VALUE},
        {KEY_3, KEY_3_VALUE}
    };

    nvs_init(key_val_pairs, sizeof(key_val_pairs) / sizeof(key_val_pairs[0]));
    nvs_write(KEY_1, 321);

    uint32_t value;
    nvs_read(KEY_2, &value);
}
 * 
 */

#define MAX_KEY_LENGTH 14


typedef struct Data
{
  char key[MAX_KEY_LENGTH];
  uint32_t value;

}NVSData;

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
NVS_write_struct_array(NVSData * array, uint8_t array_length);


/**
 * @brief 
 * 
 * @param struct_data 
 * @return NVSResult 
 */

NVSResult NVS_write_data(NVSData  struct_data);
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
