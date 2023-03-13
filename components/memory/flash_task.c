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

    void *data;
    size_t data_size;

    size_t data_drop_value;
    bool flash_formated;

    FT_can_write can_write_fnc;
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
    .data = NULL,
    .data_size = 0,
    .data_drop_value = 0,
    .flash_formated = false,
    .can_write_fnc = NULL,
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
    if (gb.data != NULL) {
        free(gb.data);
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

static void init() {
    if (FLASH_init(1) != FLASH_OK) {
        report_error(FT_INIT_ERROR);
        terminate_task();
    }
    // sometimes above 2/3 used memory, writing to flash take more than 150ms
    gb.flash.max_size = FLASH_get_total_size() * 3 / 5;
    gb.flash.wrote_size = 0;
}

static void format() {
    if (FLASH_format() != FLASH_OK) {
        report_error(FT_FORMAT);
        termninate_task();
    }

}

static void open() {
    gb.flash.file = fopen(FLASH_PATH, "a");
    if (gb.flash.file == NULL) {
        report_error(FT_FILE_OPEN);
        terminate_task();
    }
}

static void write(void *data, size_t size) {
    if (gb.flash.max_size <= gb.flash.wrote_size) {
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

static void wait_until_erase_condition(void) {
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
    ESP_LOGI(TAG, "RUNNING FLASH TASK");
    init();

    wait_until_erase_condition();  // Blocking till required condition
    ESP_LOGI(TAG, "Formatting flash");
    open();  // Fotmating flash.
    gb.flash_formated = true;

    while (1) {
        if (uxQueueMessagesWaiting(gb.queue) > gb.data_drop_value) {
            while (uxQueueMessagesWaiting(gb.queue) > 0) {
                xQueueReceive(gb.queue, gb.data, portMAX_DELAY);
                write(gb.data, gb.data_size);
                ESP_LOGI(TAG, "FLASH WRITE");
            }
            fflush(gb.flash.file);
        }

        check_termiate_condition();

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}


bool FT_init(flash_task_cfg_t *cfg) {
    if (gb.task != NULL) {
        return false;
    }

    if (cfg->can_write_to_flash_fnc == NULL) {
        ESP_LOGE(TAG, "One or more intrface functions is equal to NULL");
        return false;
    }

    gb.queue = xQueueCreate(cfg->queue_size, cfg->data_size);
    if (gb.queue == NULL) {
        return false;
    }

    gb.data = malloc(cfg->data_size);
    if (gb.data == NULL) {
        return false;
    }

    gb.events = xEventGroupCreate();
    if (gb.events == NULL) {
        free(gb.data);
        return false;
    }

    gb.data_size = cfg->data_size;
    gb.data_drop_value = cfg->queue_size * DROP_VALUE_RATIO;
    gb.can_write_fnc = cfg->can_write_to_flash_fnc;
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
        free(gb.data);
        return false;
    }

    return true;
}

bool FT_send_data(void *data) {
    if (gb.can_write_fnc() == false) {
        return false;
    }

    if (gb.flash_formated == false) {
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

void FT_erase_and_run_loop(void) {
    if (gb.events == NULL) {
        return;
    }

    xEventGroupSetBits(gb.events, EVENT_ERASE);
}

void FT_terminate(void) {
    if (gb.events == NULL) {
        return;
    }

    if (gb.flash_formated == false) {
        terminate_task();
    }

    xEventGroupSetBits(gb.events, EVENT_TERMINATE);
}
