// Copyright 2022 PWrInSpace Kuba
#include <memory.h>
#include "processing_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define TAG "SENSORS"

static struct {
    sensors_read sensors_read_fnc;
    sensors_process sensors_process_fnc;

    size_t data_buffer_size;
    void *data_buffer;
    SemaphoreHandle_t data_mutex;
    TaskHandle_t task_handle;

} gb = {
    .sensors_read_fnc = NULL,
    .sensors_process_fnc = NULL,
    .data_buffer_size = 0,
    .data_buffer = NULL,
    .data_mutex = NULL,
};

bool sensors_get_data(void *buffer, size_t buffer_size, uint32_t timeout_ms) {
    if (gb.data_mutex == NULL) {
        return false;
    }

    if (gb.data_buffer == NULL || gb.data_mutex == NULL) {
        return false;
    }

    if (buffer_size != gb.data_buffer_size) {
        return false;
    }

    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout_ms)) == pdFAIL) { // TODO: check return value
        ESP_LOGE(TAG, "Semaphore error");
        return false;
    }
    memcpy(buffer, gb.data_buffer, buffer_size);
    xSemaphoreGive(gb.data_mutex);

    return true;
}

bool sensors_change_process_function(sensors_process fnc, uint32_t timeout_ms) {
    if (gb.data_mutex == NULL) {
        return false;
    }

    if (fnc == NULL) {
        return false;
    }

    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout_ms)) == pdFAIL) {
        return false;
    }

    gb.sensors_process_fnc = fnc;
    xSemaphoreGive(gb.data_mutex);

    return true;
}

bool sensors_remove_process_function(uint32_t timeout) {
    if (gb.data_mutex == NULL) {
        return false;
    }

    if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(timeout)) == pdFAIL) {
        return false;
    }

    gb.sensors_process_fnc = NULL;
    xSemaphoreGive(gb.data_mutex);

    return true;

}


static void read_data_from_sensors(void *data_buffer) {
    if (gb.sensors_read_fnc != NULL) {
        gb.sensors_read_fnc(data_buffer);
    }
}

static void process_data(void *data_buffer) {
    if (gb.sensors_process_fnc != NULL) {
        gb.sensors_process_fnc(data_buffer);
    }
}

static void processing_task(void *arg) {
    while (1) {
        if (xSemaphoreTake(gb.data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            read_data_from_sensors(gb.data_buffer);
            process_data(gb.data_buffer);
            xSemaphoreGive(gb.data_mutex);
        } else {
            ESP_LOGE(TAG, "Semaphore error");
        }

        vTaskDelay(pdMS_TO_TICKS(SENSORS_TASK_PERIOD_MS));
    }
}

static bool initialize_rtos(void) {
    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        ESP_LOGE(TAG, "Semaphore create error");
        free(gb.data_buffer);
        return false;
    }

    xTaskCreatePinnedToCore(
        processing_task,
        "Processing",
        SENSORS_TASK_DEPTH,
        NULL,
        SENSORS_TASK_PRIORITY,
        &gb.task_handle,
        SENSORS_TASK_CPU);

    if (gb.task_handle == NULL) {
        ESP_LOGE(TAG, "Task create error");
        vSemaphoreDelete(gb.data_mutex);
        free(gb.data_buffer);
        return false;
    }

    return true;
}

bool sensors_create_task(sensors_task_cfg_t *cfg) {
    if (cfg->sensors_read_fnc == NULL) {
        ESP_LOGE(TAG, "Function");
        return false;
    }

    if (cfg->data_size == 0) {
        ESP_LOGE(TAG, "Size");
        return false;
    }

    gb.sensors_read_fnc = cfg->sensors_read_fnc;
    gb.sensors_process_fnc = cfg->sensors_process_fnc;
    gb.data_buffer_size = cfg->data_size;

    gb.data_buffer = (void*)calloc(1, gb.data_buffer_size);
    if (gb.data_buffer == NULL) {
        ESP_LOGE(TAG, "Malloc error");
        return false;
    }

    return initialize_rtos();
}