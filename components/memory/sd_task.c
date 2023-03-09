// Copyright 2022 PWr in Space, Kuba
#include "esp_log.h"
#define TAG "MEM"

#include "sdcard.h"
#include "sd_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"

// typedef struct {
//     FILE* file;
// } mem_sd_t;

// static struct {
//     uint8_t cs_pin;
//     spi_host_device_t spi_host;

//     QueueHandle_t queue;
//     size_t queue_data_size;

//     create_csv_header create_csv_header_fnc;
//     create_csv_string create_csv_string_fnc;
//     error_handler error_handler_fnc;
//     task_terminate_condition task_terminate_condition_fnc;

//     struct string {
//         char* buffer;
//         size_t size;
//     } data_buffer, path;
// } mem = {
//     .queue = NULL,
//     .create_csv_header_fnc = NULL,
//     .create_csv_string_fnc = NULL,
//     .error_handler_fnc = NULL,
//     .task_terminate_condition_fnc = NULL,
// };

// static void report_error(SD_TASK_ERR error_code) {
//     if (mem.error_handler_fnc == NULL) {
//         return;
//     }

//     mem.error_handler_fnc(error_code);
// }

// static void init(sd_card_t *sd_card, mem_sd_t *sd) {
//     bool ret = SD_init(sd_card, mem.spi_host, mem.cs_pin, SDCARD_MOUNT_POINT);
//     check_error(ret == false, "SD init error");

//     if (create_path_to_file(sd->path, sizeof(sd->path)) == false) {
//         ESP_LOGW(TAG, "CAN'T CREATE FILE PATH, USING -> %s", sd->path);
//     } else {
//         ESP_LOGI(TAG, "Using file path %s", sd->path);
//     }

//     SD_write(sd_card, sd->path, SD_HEADER, sizeof(SD_HEADER));
// }

// static void write(sd_card_t *sd_card, mem_sd_t *sd, void *data, size_t data_size) {
//     if (sd_card->mounted == false) {
//         if (SD_mount(sd_card) == false) {
//             return;
//         }
//     }

//     if (SD_is_ok(sd_card) == false) {
//         SD_remount(sd_card);
//         return;
//     }

//     if (sd->file == NULL) {
//         return;
//     }

//     if (mem.create_csv_string_fnc(
//             sd->data_buffer,
//             sizeof(sd->data_buffer),
//             data, data_size) == 0) {
//             report_error(SD_CREATE_CSV_STRING);
//     }

//     fprintf(sd->file, sd->data_buffer, sd_card->card->cid.name);
// }

// static void check_terminate_condition(void) {
//     if (mem.task_terminate_condition_fnc == NULL) {
//         return false;
//     }

//     if (mem.task_terminate_condition_fnc() == true) {
//         vQueueDelete(mem.queue);
//         mem.queue = NULL;
//         vTaskDelete(NULL);
//     }
// }



// static void sdTask(void *args) {
//     ESP_LOGI(TAG, "RUNNING SD TASK");

//     sd_card_t sd_card;
//     mem_sd_t m_sd = {.path = SD_CREATE_FILE_PREFIX(SD_FILE_NAME)};
//     void *data;
//     init(&sd_card, &m_sd);

//     while(1) {
//         if (uxQueueMessagesWaiting(mem.queue) > SD_DATA_DROP_VALUE) {
//             m_sd.file = fopen(m_sd.path, "a");
//             while (uxQueueMessagesWaiting(mem.queue) > 0) {
//                 if (xQueueReceive(mem.queue, &data, 0) == pdFALSE) {
//                     report_error(SD_QUEUE_READ);
//                 }
//                 write(&sd_card, &m_sd, &data, mem.queue_data_size);

//             }
//             fclose(m_sd.file);
//         }
//         check_terminate_condition();

//         vTaskDelay(1 / portTICK_PERIOD_MS);
//     }
// }



// bool init_sd_task(sd_task_cfg_t *cfg) {
//     if (cfg->create_csv_string_fnc == NULL || cfg->create_csv_header_fnc == NULL) {
//         ESP_LOGE(TAG, "Create csv string function is NULL");
//         return false;
//     }

//     mem.queue = xQueueCreate(SD_QUEUE_SIZE, sizeof(cfg->queue_data_size));
//     if (mem.queue == NULL) {
//         return false;
//     }

//     mem.path.buffer = malloc(cfg->path_size);
//     mem.path.size = cfg->path_size;

//     mem.cs_pin = cfg->cs_pin;
//     mem.spi_host = cfg->spi_host;
//     mem.queue_data_size = cfg->queue_data_size;
//     mem.create_csv_header_fnc = cfg->create_csv_header_fnc;
//     mem.create_csv_string_fnc = cfg->create_csv_string_fnc;
//     mem.error_handler_fnc = cfg->error_handler_fnc;
//     mem.task_terminate_condition_fnc = cfg->task_terminate_condition_fnc;

//     xTaskCreatePinnedToCore(sdTask, "sd task", 8000, NULL, 5, NULL, APP_CPU_NUM);

//     return true;
// }

// bool sendDataToSdTask(void *data) {
//     if (mem.queue == NULL) {
//         return false;
//     }

//     if (xQueueSend(mem.queue, data, 0) == pdFALSE) {
//         ESP_LOGW(TAG, "Unable to add data to sd mem.queue");
//         return false;
//     }

//     return true;
// }
