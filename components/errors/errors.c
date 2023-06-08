// Copyright 2022 PWrInSpace, Kuba
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "errors.h"
#include <memory.h>

#include "esp_log.h"
#define TAG "ERROR"

static struct {
    error_data_t errors_data[MAX_NUMBER_OF_ERRORS];
    size_t number_of_errors;
    SemaphoreHandle_t data_mutex;
} gb = {
    .data_mutex = NULL,
};


bool errors_init(error_type_t *errors_types, size_t number_of_errors) {
    if (number_of_errors > MAX_NUMBER_OF_ERRORS) {
        ESP_LOGE(TAG, "Maximum number of errors %d", number_of_errors);
        return false;
    }

    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }

    for (int i = 0; i < number_of_errors; ++i) {
        if (errors_types[i] >= MAX_NUMBER_OF_ERRORS) {
            ESP_LOGE(TAG,
                     "Errors index may be repeated -> hashing function x % MAX_NUMBER_OF_ERRORS");
            return false;
        }
    }

    gb.number_of_errors = number_of_errors;

    return true;
}

inline static uint8_t hash_function(uint8_t key) {
    return key % MAX_NUMBER_OF_ERRORS;
}

bool errors_set(error_type_t type, error_code_t code, uint32_t timeout) {
    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFALSE) {
        return false;
    }
    gb.errors_data[hash_function(type)] = code;

    xSemaphoreGive(gb.data_mutex);
    return true;
}

bool errors_add(error_type_t type, error_code_t code, uint32_t timeout) {
    if (gb.data_mutex == NULL) {
        return false;
    }

    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFALSE) {
        return false;
    }
    gb.errors_data[hash_function(type)] |= code;

    xSemaphoreGive(gb.data_mutex);
    return true;
}

error_data_t errors_get(error_type_t type) {
    error_data_t data;
    if (xSemaphoreTake(gb.data_mutex, 1000) == pdFAIL) {
        return false;
    }

    data = gb.errors_data[hash_function(type)];

    xSemaphoreGive(gb.data_mutex);

    return data;
}

bool errors_get_all(error_data_t *buffer, size_t buffer_size) {
    if (buffer_size < sizeof(gb.errors_data)) {
        return false;
    }

    if (xSemaphoreTake(gb.data_mutex, 1000) == pdFAIL) {
        return false;
    }

    memcpy(buffer, gb.errors_data, sizeof(gb.errors_data));

    xSemaphoreGive(gb.data_mutex);

    return true;
}

bool errors_reset_code(error_type_t type, error_code_t code, uint32_t timeout) {
    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFALSE) {
        return false;
    }

    gb.errors_data[hash_function(type)] &= ~code;

    xSemaphoreGive(gb.data_mutex);
    return true;
}

bool errors_reset(error_type_t type, uint32_t timeout) {
    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFALSE) {
        return false;
    }
    gb.errors_data[hash_function(type)] = 0;

    xSemaphoreGive(gb.data_mutex);
    return true;
}

bool errors_reset_all(uint32_t timeout) {
    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFALSE) {
        return false;
    }

    for (int i = 0; i < MAX_NUMBER_OF_ERRORS; ++i) {
        gb.errors_data[i] = 0;
    }

    xSemaphoreGive(gb.data_mutex);
    return true;
}