// Copyright 2022 PWr in Space, Krzysztof Gliwiński
//Edited Mateusz Kłosiński 21.02.2024
#include "flash_nvs.h"

#define TAG "NVS"

static struct {
  nvs_handle_t handle;
  bool initialized;
} nvs;

nvs_handle_t handle;
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

NVSResult NVS_init(NVSData *data_array, uint8_t length)
{

  esp_err_t res;

  res = nvs_flash_init();
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS error init %s", esp_err_to_name(res));
    return NVS_INIT_ERROR;
  }

  nvs.handle = 0;
  
  length = sizeof(*data_array)/sizeof(data_array[0]);
  if(sizeof(data_array = 0))
  {
    length = 0;
    ESP_LOGE(TAG, "data array is length 0 %s", esp_err_to_name(res));
    return NVS_INIT_ERROR;
  }

uint32_t tmp_Val;
for(int i=0; i<length; i++)
{
  NVSResult result = NVS_read_uint32t(data_array[i].key, &tmp_Val);
  if( result == NVS_OK)
  {
    ESP_LOGI(TAG, "Key %s already exists in NVS, skipping", data_array[i].key);
  }
  
  else if (result == NVS_READ_ERROR)
  {
     ESP_LOGI(TAG, "Key %s doesnt exist in NVS, saving", data_array[i].key);
     NVS_write_uint32(data_array[i].key,data_array[i].value);
  }
  else
  {
    ESP_LOGI(TAG, "NVS read encountered error");
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
  
}
  
}

NVSResult NVS_write_uint32(const char* key, uint32_t val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READWRITE, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_set_u32(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS write error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}

NVSResult NVS_read_uint32t(const char* key, uint32_t* val) {
  esp_err_t res;
  res = nvs_open("storage", NVS_READONLY, &nvs.handle);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
    return NVS_OPEN_ERROR;
  }

  res = nvs_get_u32(nvs.handle, key, val);
  if (res != ESP_OK) {
    nvs_close(nvs.handle);
    ESP_LOGE(TAG, "NVS read error %s", esp_err_to_name(res));
    return NVS_READ_ERROR;
  }

  nvs_close(nvs.handle);
  return NVS_OK;
}

/*NVSResult NVS_init(void) {
  esp_err_t res;

  res = nvs_flash_init();
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "NVS error init %s", esp_err_to_name(res));
    return NVS_INIT_ERROR;
  }

  nvs.handle = 0;
  return NVS_OK;
}
*/




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