// Copyright 2022 PWr in Space, Krzysztof Gliwiński
#include "flash.h"

#define TAG "SPIFFS"

static struct {
  esp_vfs_spiffs_conf_t conf;
  size_t total_size;
  bool initialized;
} fl;

FlashResult FLASH_init(uint8_t max_files) {
  esp_err_t err;
  if (fl.initialized == true) {
    return FLASH_ALREADY_INITIALIZED;
  }

  fl.total_size = 0;
  fl.conf.base_path = PATH;
  fl.conf.partition_label = NULL;
  fl.conf.max_files = max_files;
  fl.conf.format_if_mount_failed = false;

  err = esp_vfs_spiffs_register(&fl.conf);
  
  // FLASH_format();
  // assert(err == ESP_OK);
  if (err != ESP_OK) {
    if (err == ESP_FAIL) {
      ESP_LOGE(
          TAG,
          "Filed to mount or format, use FLASH_format(), to format partition");
    } else if (err == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Filed to find spiffs partition");
    }
    return FLASH_INIT_ERROR;
  }

  uint32_t used_size;
  err = esp_spiffs_info(fl.conf.partition_label, &fl.total_size, (size_t*) &used_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG,
             "Failed to get SPIFFS partition information, use FLASH_format(), "
             "to format partition");
    return FLASH_INIT_ERROR;
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", (int) fl.total_size,
            (int) used_size);
  }

  fl.initialized = true;
  return FLASH_OK;
}

FlashResult FLASH_write(const char* file_name, const char* data, size_t size) {
  FILE* file = NULL;

  if (fl.initialized == false) {
    ESP_LOGW(TAG, "FLASH IS NOT INITIALIZED");
    return FLASH_IS_NOT_INITIALIZED;
  }

  file = fopen(file_name, "a");
  assert(file != NULL);
  if (file == NULL) {
    return FLASH_OPEN_ERROR;
  }
  fprintf(file, data);
  fclose(file);
  return FLASH_OK;
}

FlashResult FLASH_read_all_data(const char* file_name, char* data_container,
                                size_t size) {
  FILE* file = NULL;
  size_t data_size = 0;

  if (fl.initialized == false) {
    ESP_LOGW(TAG, "FLASH IS NOT INITIALIZED");
    return FLASH_IS_NOT_INITIALIZED;
  }

  data_size = FLASH_get_used_size();

  if (data_size == 0) {
    ESP_LOGE(TAG, "no data\n");
    return FLASH_READ_NO_DATA;
  } else if (data_size > size) {
    ESP_LOGE(TAG, "user size is too smoll");
    return FLASH_READ_ALL_SIZE;
  }

  file = fopen(file_name, "r");

  assert(file != NULL);
  if (file == NULL) {
    return FLASH_OPEN_ERROR;
  }

  // while(feof(file) == 0){
  //     fgets(*data_container, data_size, file);
  //     printf("While %s\n", (*ptr_to_alloc_data));
  // }

  fread(data_container, 1, data_size, file);
  fclose(file);
  return FLASH_OK;
}

size_t FLASH_get_used_size(void) {
  esp_err_t err;

  if (fl.initialized == false) {
    return 0;
  }

  uint32_t used_size;
  err = esp_spiffs_info(fl.conf.partition_label, &fl.total_size, (size_t*) &used_size);
  if (err != ESP_OK) {
    return 0;
  }
  ESP_LOGI(TAG, "Used size %d, total size %d", (int) used_size, (int) fl.total_size);
  return used_size;
}

size_t FLASH_get_total_size(void) {
  if (fl.initialized == false) {
    return 0;
  }

  return fl.total_size;
}


FlashResult FLASH_format(void) {
  esp_err_t err;

  // if (fl.initialized == false) {
  //   ESP_LOGW(TAG, "FLASH IS NOT INITIALIZED");
  //   return FLASH_IS_NOT_INITIALIZED;
  // }
  ESP_LOGI(TAG, "Formatting");
  err = esp_spiffs_format(fl.conf.partition_label);
  if (err != ESP_OK) {
    return FLASH_FORMAT_ERROR;
  }

  ESP_LOGI(TAG, "FLASH FORMATED");
  return FLASH_OK;
}
