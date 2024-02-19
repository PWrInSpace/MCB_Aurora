// Copyright 2022 PWr in Space, Krzysztof Gliwiński
#include "flash_nvs.h"

#define TAG "NVS"

static struct {
  nvs_handle_t handle;
  bool initialized;
} nvs;

uint8_t * data_uint8t;
  int8_t *data_int8t;
  uint16_t * data_uint16t;
  int16_t * data_int16t;
  uint32_t *data_uint32t;
  int32_t *data_int32t;
  uint64_t *data_uint64t;
  int64_t *data_int64t;
  NVSDataType datatype;


NVS_write_struct_array(NVSData * array, uint8_t array_length)
{
  for(int i =0; i<array_length;i++)
  {
    NVS_write_data(array[i]);
  }
}



NVSResult NVS_write_data(NVSData data_nvs);
{
  switch(data_nvs.datatypee)
  {
    case data_nvs.datatype = uint8_t;
    {
      NVS_write_uint8(data_nvs.key,data_nvs.data_uint8t);
    }

    case sdata_nvs.datatype= int8_t;
    {
      NVS_write_int8(data_nvs.key,data_nvs.data_int8t);
    }

     case data_nvs.datatype = uint16_t;
    {
      NVS_write_uint16(data_nvs.key,data_nvs.data_uint16t);
    }

    case data_nvs.datatype = int16_t;
    {
      NVS_write_int16(data_nvs.key,data_nvs.data_int16t);
    }

    case data_nvs.datatype = uint32_t;
    {
      NVS_write_uint32(data_nvs.key,data.data_uint32t);
    }

    case data_nvs.datatype = int32_t;
    {
      NVS_write_int32(data_nvs.key,data.data_int32t);
    }
    case data_nvs.datatype = uint64_t;
    {
      NVS_write_uint64(data_nvs.key,data.data_uint64t);
    }
    case data_nvs.datatype = int64_t;
    {
      NVS_write_int64(data_nvs.key,data.data_int64t);
    }
    break;

  }
}


NVSResult NVS_init(void) {
  esp_err_t res;

  res = nvs_flash_init();
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS error init %s", esp_err_to_name(res));
    return NVS_INIT_ERROR;
  }

  nvs.handle = 0;
  return NVS_OK;
}

NVSResult NVS_write_uint8(const char* key, uint8_t val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READWRITE, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_set_u8(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS write error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}

NVSResult NVS_read_uint8(const char* key, uint8_t* val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READONLY, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_get_u8(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS read error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}

NVSResult NVS_read_uint16(const char* key, uint16_t* val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READONLY, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_get_u16(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS read error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}

NVSResult NVS_write_uint16(const char* key, uint16_t val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READWRITE, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_set_u16(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS write error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}
