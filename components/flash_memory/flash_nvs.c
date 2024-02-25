// Copyright 2022 PWr in Space, Krzysztof Gliwiński
// Edited Mateusz Kłosiński 21.02.2024
#include "flash_nvs.h"

#define TAG "NVS"

static struct {
    nvs_handle_t handle;
    bool initialized;
} nvs;

NVSResult NVS_init(NVSData* data_array, size_t length) {
    esp_err_t res;

    res = nvs_flash_init();
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "NVS error init %s", esp_err_to_name(res));
        return NVS_INIT_ERROR;
    }

    if (length == 0 || data_array == NULL) {
        length = 0;
        ESP_LOGE(TAG, "data array is length 0 %s", esp_err_to_name(res));
        return NVS_INIT_ERROR;
    }
int32_t tmp_Val;
    for (int i = 0; i < length; i++) {
        NVSResult result = NVS_read_int32t(data_array[i].key, &tmp_Val);
        if (result == NVS_OK) {
            ESP_LOGI(TAG, "Key %s already exists in NVS, skipping", data_array[i].key);
        } else if (result == NVS_NO_MATCHING_KEY) {
            ESP_LOGI(TAG, "Key %s doesnt exist in NVS, saving", data_array[i].key);
            NVS_write_int32(data_array[i].key, data_array[i].value);
        } else {
            ESP_LOGI(TAG, "NVS read encountered error");
            return NVS_READ_ERROR;
        }
    }
    nvs_commit(nvs.handle);
    nvs_close(nvs.handle);

    return NVS_OK;
}

NVSResult NVS_write_int32(const char* key, int32_t val) {
    esp_err_t res;

    res = nvs_open("storage", NVS_READWRITE, &nvs.handle);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
        return NVS_OPEN_ERROR;
    }

    res = nvs_set_i32(nvs.handle, key, val);
    if (res != ESP_OK) {
        nvs_close(nvs.handle);
        ESP_LOGE(TAG, "NVS write error %s", esp_err_to_name(res));
        return NVS_READ_ERROR;
    }

    nvs_close(nvs.handle);
    return NVS_OK;
}

NVSResult NVS_read_int32t(const char* key, int32_t* val) {
    esp_err_t res;
    res = nvs_open("storage", NVS_READONLY, &nvs.handle);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "NVS open error %s", esp_err_to_name(res));
        return NVS_OPEN_ERROR;
    }

    res = nvs_get_i32(nvs.handle, key, val);

    if(res == ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(nvs.handle);
        ESP_LOGE(TAG, "NVS no matching key %s", esp_err_to_name(res));
        return NVS_NO_MATCHING_KEY;
    }

    if (res != ESP_OK) {
        nvs_close(nvs.handle);
        ESP_LOGE(TAG, "NVS read error %s", esp_err_to_name(res));
        return NVS_READ_ERROR;
    }

    nvs_close(nvs.handle);
    return NVS_OK;
}
