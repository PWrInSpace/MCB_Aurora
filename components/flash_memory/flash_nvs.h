// Copyright 2022 PWr in Space, Krzysztof Gliwiński
//02.2024 edited by Mateusz Kłosiński
#pragma once

#include <stdint.h>

#include "nvs_flash.h"
#include "esp_log.h"



#define MAX_KEY_LENGTH 14


typedef struct Data
{
  char key[MAX_KEY_LENGTH];
  uint8_t * data_uint8t;
  int8_t *data_int8t;
  uint16_t * data_uint16t;
  int16_t * data_int16t;
  uint32_t *data_uint32t;
  int32_t *data_int32t;
  uint64_t *data_uint64t;
  int64_t *data_int64t;
  NVSDataType datatype;
}NVSData;

typedef enum {
  NVS_OK,
  NVS_INIT_ERROR,
  NVS_OPEN_ERROR,
  NVS_READ_ERROR,
} NVSResult;

typedef enum {
  uint8_t,
  int8_t,
  uint16_t,
  int16_t,
  uint32_t, 
  int32_t, s
  uint64_t,
  int64_t
} NVSDataType;

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
