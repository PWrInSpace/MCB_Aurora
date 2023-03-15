// Copyright 2022 PWr in Space, Kuba
#include "esp_log.h"
#define TAG "SDT"

#include "sd_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"

static struct {
    sd_card_t sd_card;

    QueueHandle_t data_queue;
    QueueHandle_t log_queue;

    char data_buffer[SD_DATA_BUFFER_MAX_SIZE];
    char log_buffer[SD_LOG_BUFFER_MAX_SIZE];

    char data_path[SD_PATH_SIZE];
    char log_path[SD_PATH_SIZE];

    error_handler error_handler_fnc;
} mem;

static void report_error(SD_TASK_ERR error_code) {
    if (mem.error_handler_fnc == NULL) {
        return;
    }

    mem.error_handler_fnc(error_code);
}

static void write_to_sd(FILE * file, char *data, size_t size) {
    if (mem.sd_card.mounted == false) {
        if (SD_mount(&mem.sd_card) == false) {
            return;
        }
    }

    if (SD_is_ok(&mem.sd_card) == false) {
        SD_remount(&mem.sd_card);
        return;
    }

    if (file == NULL) {
        return;
    }

    fwrite(data, size, 1, file);
}

// static void check_terminate_condition(void) {
//     // if (mem.task_terminate_condition_fnc == NULL) {
//     //     return false;
//     // }

//     // if (mem.task_terminate_condition_fnc() == true) {
//     //     vQueueDelete(mem.queue);
//     //     mem.queue = NULL;
//     //     vTaskDelete(NULL);
//     // }
// }

static void sdTask(void *args) {
    ESP_LOGI(TAG, "RUNNING SD TASK");
    while(1) {
        if (uxQueueMessagesWaiting(mem.data_queue) > SD_DATA_DROP_VALUE) {
            FILE* data_file = fopen(mem.data_path, "a");
            ESP_LOGI(TAG, "SD save");
            while (uxQueueMessagesWaiting(mem.data_queue) > 0) {
                if (xQueueReceive(mem.data_queue, &mem.data_buffer, 0) == pdFALSE) {
                    report_error(SD_QUEUE_READ);
                } else {
                    write_to_sd(data_file, mem.data_buffer, sizeof(mem.data_buffer));
                }
                // vTaskDelay(pdMS_TO_TICKS(1));
            }
            fclose(data_file);
        }

        if (xQueueReceive(mem.log_queue, &mem.log_buffer, 0) == pdTRUE) {
            SD_write(&mem.sd_card, mem.log_path, mem.log_buffer, sizeof(mem.log_buffer));
        }

        // check_terminate_condition();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool create_unique_path(char* path, size_t size) {
    char temp_path[SD_PATH_SIZE] = {0};
    int ret = 0;
    ESP_LOGI(TAG, "%s", path);
    for (int i = 0; i < 1000; ++i) {
        ret = snprintf(temp_path, sizeof(temp_path), SD_MOUNT_POINT"/%s%d.txt", path, i);
        if (ret == SD_PATH_SIZE) {
            return false;
        }

        if (SD_file_exists(temp_path) == false) {
            memcpy(path, temp_path, size);
            return true;
        }
    }
    return false;
}

bool SDT_init(sd_task_cfg_t *task_cfg) {
    mem.data_queue = xQueueCreate(SD_DATA_QUEUE_SIZE, sizeof(char[SD_DATA_BUFFER_MAX_SIZE]));
    if (mem.data_queue == NULL) {
        return false;
    }

    mem.log_queue = xQueueCreate(SD_LOG_QUEUE_SIZE, sizeof(char[SD_LOG_BUFFER_MAX_SIZE]));
    if (mem.log_queue == NULL) {
        vQueueDelete(mem.data_queue);
        mem.data_queue = NULL;
        return false;
    }


    sd_card_config_t card_cfg = {
        .spi_host = task_cfg->spi_host,
        .cs_pin = task_cfg->cs_pin,
        .mount_point = SDCARD_MOUNT_POINT,
    };

    bool ret = SD_init(&mem.sd_card, &card_cfg);
    if (ret == false) {
        // vQueueDelete(mem.data_queue);
        // mem.data_queue = NULL;
        // vQueueDelete(mem.log_queue);
        // mem.log_queue = NULL;
        ESP_LOGW(TAG, "Unable to initialize SD card");
        // return false;
    }

    memcpy(mem.data_path, task_cfg->data_path, task_cfg->data_path_size);
    memcpy(mem.log_path, task_cfg->log_path, task_cfg->log_path_size);

    if (create_unique_path(mem.data_path, sizeof(mem.data_path) )== false) {
        ESP_LOGE(TAG, "Unable to create unique path");
    }

    if (create_unique_path(mem.log_path, sizeof(mem.log_path)) == false) {
        ESP_LOGE(TAG, "Unable to create unique path");
    }

    ESP_LOGI(TAG, "Using data path %s", mem.data_path);
    ESP_LOGI(TAG, "Using log path %s", mem.log_path);

    xTaskCreatePinnedToCore(sdTask, "sd task", 8000, NULL, 5, NULL, APP_CPU_NUM);

    return true;
}

bool SDT_send_data(char *data, size_t data_size) {
    if (mem.data_queue == NULL) {
        return false;
    }

    if (data_size > SD_DATA_BUFFER_MAX_SIZE) {
        return false;
    }

    if (xQueueSend(mem.data_queue, data, 0) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
        return false;
    }

    return true;
}

bool SDT_send_log(char *data, size_t data_size) {
    if (mem.log_queue == NULL) {
        return false;
    }

    if (data_size > SD_DATA_BUFFER_MAX_SIZE) {
        return false;
    }

    if (xQueueSend(mem.log_queue, data, 0) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
        return false;
    }

    return true;
}
