// Copyright 2022 PWr in Space, Kuba

#include "flash_task.h"
#include "flash.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#define TAG "FT"

#define EVENT_ERASE (1 << 0)
#define EVENT_TERMINATE (1 << 1)



static struct {
    TaskHandle_t task;
    QueueHandle_t queue;
    EventGroupHandle_t events;    // TO DO  change to task notification

    void *data_from_queue;
    size_t data_size;

    bool flash_enable;

    FT_error_handler error_handler_fnc;

    struct {
        uint32_t wrote_size;
        uint32_t max_size;
        FILE* file;
    } flash;
} gb = {
    .task = NULL,
    .queue = NULL,
    .events = NULL,
    .data_from_queue = NULL,
    .data_size = 0,
    .flash_enable = false,
    .error_handler_fnc = NULL,
    .flash.wrote_size = 0,
    .flash.max_size = 0,
    .flash.file = 0,
};

static void report_error(FT_ERROR_CODE error_code) {
    ESP_LOGE(TAG, "ERROR CODE %d", error_code);

    if (gb.error_handler_fnc == NULL) {
        return;
    }

    gb.error_handler_fnc(error_code);
}

static void terminate_task(void) {
    ESP_LOGE(TAG, "TERMINATING FLASH TASK !!!");
    if (gb.data_from_queue != NULL) {
        free(gb.data_from_queue);
    }

    if (gb.flash.file != NULL) {
        fclose(gb.flash.file);
    }

    vEventGroupDelete(gb.events);
    gb.events = NULL;
    vQueueDelete(gb.queue);
    gb.queue = NULL;
    vTaskDelete(NULL);
}

static bool init(void) {
    if (FLASH_init(1) != FLASH_OK) {
        report_error(FT_INIT_ERROR);
        ESP_LOGW(TAG, "Formating flash");
        if (FLASH_format() != FLASH_OK) {
            return false;
        }

        if (FLASH_init(1) != FLASH_OK) {
            return false;
        }
    }

    // sometimes above 2/3 used memory, writing to flash take more than 150ms
    gb.flash.max_size = FLASH_get_total_size() * 3 / 5;
    gb.flash.wrote_size = FLASH_get_used_size();
    return true;
}

static void open() {
    gb.flash.file = fopen(FLASH_PATH, "a");
    if (gb.flash.file == NULL) {
        report_error(FT_FILE_OPEN);
        terminate_task();
    }
}

static void write(void *data, size_t size) {
    if (gb.flash.wrote_size + size > gb.flash.max_size) {
        ESP_LOGW(TAG, "MAX SIZE");
        report_error(FT_FILE_FULL);
        terminate_task();
        return;
    }

    size_t ret = fwrite(data, size, 1, gb.flash.file);
    if (ret != 1) {
        ESP_LOGE(TAG, "Flash write error");
        report_error(FT_WRITE);
    } else {
        gb.flash.wrote_size += size;
    }
}

static void wait_until_start_condition(void) {
    EventBits_t bits = 0x00;

    while (!(bits & EVENT_ERASE)) {  // NAND GATE
        vTaskDelay(pdMS_TO_TICKS(100));
        bits = xEventGroupGetBits(gb.events);
    }
}

static void check_termiate_condition(void) {
    if (xEventGroupGetBits(gb.events) & EVENT_TERMINATE) {  // AND GATE
        terminate_task();
    }
}

static void flash_task(void *arg) {
    wait_until_start_condition();  // Blocking till required condition
    open();
    gb.flash_enable = true;

    while (1) {
        if (uxQueueMessagesWaiting(gb.queue) > FLASH_DROP_VALUE) {
            ESP_LOGW(TAG, "WRITING TO FLASH");
            while (uxQueueMessagesWaiting(gb.queue) > 0) {
                xQueueReceive(gb.queue, gb.data_from_queue, portMAX_DELAY);
                write(gb.data_from_queue, gb.data_size);
            }
            fflush(gb.flash.file);
        }

        check_termiate_condition();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


bool FT_init(flash_task_cfg_t *cfg) {
    if (gb.task != NULL) {
        return false;
    }
    gb.queue = xQueueCreate(FLASH_QUEUE_SIZE, cfg->data_size);
    if (gb.queue == NULL) {
        return false;
    }

    gb.data_from_queue = malloc(cfg->data_size);
    if (gb.data_from_queue == NULL) {
        return false;
    }

    gb.events = xEventGroupCreate();
    if (gb.events == NULL) {
        free(gb.data_from_queue);
        return false;
    }

    if (init() == false) {
        return false;
    }

    gb.data_size = cfg->data_size;
    gb.error_handler_fnc = cfg->error_handler_fnc;

    xTaskCreatePinnedToCore(
        flash_task,
        "flash task",
        cfg->stack_depth,
        NULL,
        cfg->priority,
        &gb.task,
        cfg->core_id);

    if (gb.task == NULL) {
        free(gb.data_from_queue);
        return false;
    }

    return true;
}

bool FT_send_data(void *data) {
    if (gb.flash_enable == false) {
        return false;
    }

    if (gb.queue == NULL) {
        return false;
    }

    if (xQueueSend(gb.queue, data, 10) == pdFALSE) {
        ESP_LOGW(TAG, "Unable to add data to flash queue");
        return false;
    }

    return true;
}

void FT_start_loop(void) {
    if (gb.events == NULL) {
        return;
    }

    xEventGroupSetBits(gb.events, EVENT_ERASE);
}

void FT_format() {
    if (FLASH_format() != FLASH_OK) {
        report_error(FT_FORMAT);
        terminate_task();
    }
}

void FT_terminate_task(void) {
    if (gb.events == NULL) {
        return;
    }

    if (gb.flash_enable == false) {
        terminate_task();
    }

    xEventGroupSetBits(gb.events, EVENT_TERMINATE);
}

bool FT_is_enable(void) {
    return gb.flash_enable;
}