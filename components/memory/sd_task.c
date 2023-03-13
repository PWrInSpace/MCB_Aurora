// Copyright 2022 PWr in Space, Kuba
#include "esp_log.h"
#define TAG "MEM"

#include "sdcard.h"
#include "sd_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"

static struct {
    sd_card_t sd_card;

    QueueHandle_t data_queue;
    QueueHandle_t log_queue;

    void *data;
    size_t data_size;

    error_handler error_handler_fnc;

    struct string {
        char* buffer;
        size_t size;
    } data_buffer, path;
} mem;

static void report_error(SD_TASK_ERR error_code) {
    if (mem.error_handler_fnc == NULL) {
        return;
    }

    mem.error_handler_fnc(error_code);
}

static void write(sd_card_t *sd_card, char *data) {
    if (sd_card->mounted == false) {
        if (SD_mount(sd_card) == false) {
            return;
        }
    }

    if (SD_is_ok(sd_card) == false) {
        SD_remount(sd_card);
        return;
    }

    // if (sd->file == NULL) {
    //     return;
    // }

    fprintf(sd->file, sd->data_buffer, sd_card->card->cid.name);
}

static void check_terminate_condition(void) {
    // if (mem.task_terminate_condition_fnc == NULL) {
    //     return false;
    // }

    // if (mem.task_terminate_condition_fnc() == true) {
    //     vQueueDelete(mem.queue);
    //     mem.queue = NULL;
    //     vTaskDelete(NULL);
    // }
}

static void sdTask(void *args) {
    ESP_LOGI(TAG, "RUNNING SD TASK");

    init(&sd_card, &m_sd);

    while(1) {
        if (uxQueueMessagesWaiting(mem.queue) > SD_DATA_DROP_VALUE) {
            m_sd.file = fopen(m_sd.path, "a");
            while (uxQueueMessagesWaiting(mem.queue) > 0) {
                if (xQueueReceive(mem.queue, &data, 0) == pdFALSE) {
                    report_error(SD_QUEUE_READ);
                }
                write(&sd_card, &m_sd, &data, mem.queue_data_size);

            }
            fclose(m_sd.file);
        }
        check_terminate_condition();

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


static void init_sd_card(sd_card_config_t *cfg) {
    bool ret = SD_init(sd_card, mem.spi_host, mem.cs_pin, SDCARD_MOUNT_POINT);
    check_error(ret == false, "SD init error");

    if (create_path_to_file(sd->path, sizeof(sd->path)) == false) {
        ESP_LOGW(TAG, "CAN'T CREATE FILE PATH, USING -> %s", sd->path);
    } else {
        ESP_LOGI(TAG, "Using file path %s", sd->path);
    }

    SD_write(sd_card, sd->path, SD_HEADER, sizeof(SD_HEADER));
}

bool init_sd_task(sd_task_cfg_t *cfg) {
    mem.queue = xQueueCreate(SD_QUEUE_SIZE, sizeof(cfg->queue_data_size));
    if (mem.queue == NULL) {
        return false;
    }

    mem.path.buffer = malloc(cfg->path_size);
    mem.path.size = cfg->path_size;

    mem.data_size = cfg->queue_data_size;
    mem.error_handler_fnc = cfg->error_handler_fnc;

    xTaskCreatePinnedToCore(sdTask, "sd task", 8000, NULL, 5, NULL, APP_CPU_NUM);

    return true;
}

bool send_data_to_sd_task(void *data) {
    if (mem.queue == NULL) {
        return false;
    }

    if (xQueueSend(mem.queue, data, 0) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
        return false;
    }

    return true;
}
