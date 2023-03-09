// Copyright 2022 PWr in Space, Kuba

#include "flash_task.h"
#include "flash.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"

#define TAG "MEM"

typedef struct {
    bool formated;
    uint32_t wrote_size;
    uint32_t max_size;
    FILE* file;
} mem_flash_t;

static struct {
    QueueHandle_t queue;
    size_t data_size;
    size_t data_drop_value;
    bool flash_formated;

    FT_can_write can_write_fnc;
    FT_erase_condition erase_condition_fnc;
    FT_error_handler error_handler_fnc;
    FT_terminate_codnition termiante_condition_fnc;
} mem = {
    .queue = NULL,
    .flash_formated = false,
};

static void report_error(FT_ERROR_CODE error_code) {
    ESP_LOGE(TAG, "ERROR CODE %d", error_code);

    if (mem.error_handler_fnc == NULL) {
        return;
    }

    mem.error_handler_fnc(error_code);
}

static void terminate_task(void) {
    ESP_LOGE(TAG, "TERMINATING FLASH TASK !!!");
    vQueueDelete(mem.queue);
    mem.queue = NULL;
    vTaskDelete(NULL);
}

static void init(mem_flash_t *fl) {
    if (FLASH_init(1) != FLASH_OK) {
        report_error(FT_INIT_ERROR);
        terminate_task();
    }
    // sometimes above 2/3 used memory, writing to flash take more than 150ms
    fl->max_size = FLASH_get_total_size() * 3 / 5;
    fl->wrote_size = 0;
}

static void open(mem_flash_t *fl) {
    FLASH_format();

    fl->file = fopen(FLASH_PATH, "a");
    if (fl->file == NULL) {
        report_error(FT_FILE_OPEN);
        terminate_task();
    }
}

static void write(mem_flash_t *fl, void *data, size_t size) {
    if (fl->max_size <= fl->wrote_size) {
        ESP_LOGW(TAG, "MAX SIZE");
        report_error(FT_FILE_FULL);
        terminate_task();
        return;
    }

    size_t ret = fwrite(data, size, 1, fl->file);
    if (ret != 1) {
        ESP_LOGE(TAG, "Flash write error");
        report_error(FT_WRITE);
    } else {
        fl->wrote_size += size;
    }
}

static void wait_until_erase_condition(void) {
    while (mem.erase_condition_fnc() == false) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void check_termiate_condition(void) {
    if (mem.termiante_condition_fnc == NULL) {
        return;
    }

    if (mem.termiante_condition_fnc() == true) {
        terminate_task();
    }
}

static void flashTask(void *arg) {
    ESP_LOGI(TAG, "RUNNING FLASH TASK");
    mem_flash_t m_flash;
    init(&m_flash);

    wait_until_erase_condition();  // Blocking till required condition

    open(&m_flash);  // Fotmating flash.
    mem.flash_formated = true;

    void *data;
    while (1) {
        if (uxQueueMessagesWaiting(mem.queue) > mem.data_drop_value) {
            while (uxQueueMessagesWaiting(mem.queue) > 0) {
                xQueueReceive(mem.queue, &data, portMAX_DELAY);
                write(&m_flash, &data, mem.data_size);
            }
            fflush(m_flash.file);
        }

        check_termiate_condition();

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


bool init_flash_task(flash_task_cfg_t *cfg) {
    if (cfg->can_write_to_flash_fnc == NULL ||
        cfg->erase_codnition_fnc == NULL) {
            ESP_LOGE(TAG, "One or more intrface functions is equal to NULL");
            return false;
    }

    mem.queue = xQueueCreate(cfg->queue_size, cfg->data_size);
    if (mem.queue == NULL) {
        return false;
    }

    mem.data_size = cfg->data_size;
    mem.data_drop_value = cfg->queue_size * DROP_VALUE_RATIO;
    mem.can_write_fnc = cfg->can_write_to_flash_fnc;
    mem.erase_condition_fnc = cfg->erase_codnition_fnc;
    mem.termiante_condition_fnc = cfg->terminate_codnition_fnc;
    mem.error_handler_fnc = cfg->error_handler_fnc;
    xTaskCreatePinnedToCore(flashTask, "flash task", 8000, NULL, 5, NULL, APP_CPU_NUM);
    return true;
}

bool send_data_to_flash_task(void *data) {
    if (mem.can_write_fnc() == false) {
        return false;
    }

    if (mem.flash_formated == false) {
        return false;
    }

    if (mem.queue == NULL) {
        return false;
    }

    if (xQueueSend(mem.queue, data, 10) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to flash queue");
        return false;
    }

    return true;
}
