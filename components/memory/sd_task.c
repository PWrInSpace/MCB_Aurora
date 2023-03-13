// Copyright 2022 PWr in Space, Kuba
#include "esp_log.h"
#define TAG "MEM"

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

    error_handler error_handler_fnc;
} mem;

static void report_error(SD_TASK_ERR error_code) {
    if (mem.error_handler_fnc == NULL) {
        return;
    }

    mem.error_handler_fnc(error_code);
}

// static void write(sd_card_t *sd_card, char *data) {
//     if (sd_card->mounted == false) {
//         if (SD_mount(sd_card) == false) {
//             return;
//         }
//     }

//     if (SD_is_ok(sd_card) == false) {
//         SD_remount(sd_card);
//         return;
//     }

//     // if (sd->file == NULL) {
//     //     return;
//     // }

//     fprintf(sd->file, sd->data_buffer, sd_card->card->cid.name);
// }

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
    bool ret = SD_init(&mem.sd_card, cfg);

    if (create_path_to_file(sd->path, sizeof(sd->path)) == false) {
        ESP_LOGW(TAG, "CAN'T CREATE FILE PATH, USING -> %s", sd->path);
    } else {
        ESP_LOGI(TAG, "Using file path %s", sd->path);
    }
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

    init_sd_card(&card_cfg);

    xTaskCreatePinnedToCore(sdTask, "sd task", 8000, NULL, 5, NULL, APP_CPU_NUM);

    return true;
}

bool SDT_send_data(char *data, size_t data_size) {
    if (mem.queue == NULL) {
        return false;
    }

    if (data_size > SD_DATA_BUFFER_MAX_SIZE) {
        return false;
    }

    if (xQueueSend(mem.queue, data, 0) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
        return false;
    }

    return true;
}
