#include <memory.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "recovery_task_config.h"
#include "basic_task.h"
#include "commands.h"
#include "errors_config.h"
#include "esp_log.h"
#include "i2c.h"

#define TAG "RECOVERY"

static struct {
    basic_task_t task;
    recovery_data_t recovery_data;
    SemaphoreHandle_t processing_mutex;
    recovery_task_process_fnc process_fnc;
} gb = {
    .task = {0},
    .recovery_data = {0},
    .processing_mutex = NULL,
    .process_fnc = NULL,
};

bool recovery_send_cmd(uint32_t command, int32_t payload) {
    cmd_message_t message = {
        .cmd.command = command,
        .cmd.payload = payload,
    };
    return i2c_com_write(RECOVERY_ADDRESS, message.raw[0], &message.raw[1],
        sizeof(message.raw) - sizeof(message.raw[0]));
}

static bool recovery_read_data(uint8_t* message, size_t len) {
    return i2c_com_only_read(RECOVERY_ADDRESS, message, len);
}

static void recovery_process_data(void) {
    // Workaround - when recovery receive error occurs some data are updated with trash
    recovery_data_t copy = gb.recovery_data;

    if (recovery_read_data((uint8_t*)&gb.recovery_data, sizeof(gb.recovery_data)) == false) {
        ESP_LOGE(TAG, "RECOVERY RECEIVE ERROR");
        errors_set(ERROR_TYPE_RECOVERY, ERROR_RECOV_RECEIVE, 100);
        gb.recovery_data = copy;
    }

    rocket_data_update_recovery(&gb.recovery_data);

    xSemaphoreTake(gb.processing_mutex, portMAX_DELAY);
    if (gb.process_fnc != NULL) {
        gb.process_fnc(&gb.recovery_data);
    }
    xSemaphoreGive(gb.processing_mutex);
}

bool recovery_change_process_fnc(recovery_task_process_fnc fnc) {
    if (xSemaphoreTake(gb.processing_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return false;
    }

    gb.process_fnc = fnc;

    xSemaphoreGive(gb.processing_mutex);

    return true;
}

bool recovery_remove_process_fnc(void) {
    if (xSemaphoreTake(gb.processing_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return false;
    }

    gb.process_fnc = NULL;

    xSemaphoreGive(gb.processing_mutex);

    return true;
}

bool initialize_recovery(void) {
    gb.processing_mutex = xSemaphoreCreateMutex();
    if (gb.processing_mutex == NULL) {
        return false;
    }

    memset(&gb.recovery_data, 0, sizeof(gb.recovery_data));

    basic_task_cfg_t task_cfg = {
        .process_fnc = recovery_process_data,
        .priority = CONFIG_RECOVERY_TASK_PRIORITY,
        .core_id = CONFIG_RECOVERY_TASK_CPU,
        .stack_depth = CONFIG_RECOVERY_TASK_DEPTH,
        .task_delay = CONFIG_RECOVERY_TASK_PERIOD_MS,
    };

    if (basic_task_create(&task_cfg, &gb.task) == false) {
        return false;
    }

    return true;
}