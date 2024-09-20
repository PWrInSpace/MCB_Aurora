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
    bool leftStatus = true;
    bool rightStatus = true;

    leftStatus = i2c_com_write(RECOVERY_ADDRESS_LEFT, &message.raw[0],sizeof(message.raw));
    rightStatus = i2c_com_write(RECOVERY_ADDRESS_RIGHT, &message.raw[0],sizeof(message.raw));
    return leftStatus | rightStatus;
}

static bool recovery_read_data(recovery_data_t* message, size_t len) {
   recovery_data_t leftRecoveryData;
   recovery_data_t rightRecoveryData;
   bool leftStatus = true;
   bool rightStatus = true;
   if(i2c_com_only_read(RECOVERY_ADDRESS_LEFT, (uint8_t*)&leftRecoveryData, len) == false){
    ESP_LOGE(TAG, "LEFT RECOVERY ESP RECEIVER ERROR");
    memset(&leftRecoveryData,0,sizeof(leftRecoveryData));
    leftStatus = false;
    }
    if(i2c_com_only_read(RECOVERY_ADDRESS_RIGHT, (uint8_t*)&rightRecoveryData, len) == false){
    ESP_LOGE(TAG, "RIGHT RECOVERY ESP RECEIVER ERROR");
    memset(&rightRecoveryData,0,sizeof(rightRecoveryData));
    rightStatus = false;
    }
    gb.recovery_data.isArmed = leftRecoveryData.isArmed | rightRecoveryData.isArmed;
    gb.recovery_data.isTeleActive = leftRecoveryData.isTeleActive | rightRecoveryData.isTeleActive;
    gb.recovery_data.easyMiniFirstStage = leftRecoveryData.easyMiniFirstStage | rightRecoveryData.easyMiniFirstStage;
    gb.recovery_data.easyMiniSecondStage = leftRecoveryData.easyMiniSecondStage | rightRecoveryData.easyMiniSecondStage;
    gb.recovery_data.telemetrumFirstStage = leftRecoveryData.telemetrumFirstStage | rightRecoveryData.telemetrumFirstStage;
    gb.recovery_data.telemetrumSecondStage = leftRecoveryData.telemetrumSecondStage | rightRecoveryData.telemetrumSecondStage;
    gb.recovery_data.firstStageDone = leftRecoveryData.firstStageDone | rightRecoveryData.firstStageDone;
    gb.recovery_data.secondStageDone = leftRecoveryData.secondStageDone | rightRecoveryData.secondStageDone;
    gb.recovery_data.firstStageContinouity = leftRecoveryData.firstStageContinouity | rightRecoveryData.firstStageContinouity;
    gb.recovery_data.secondStageContinouity = leftRecoveryData.secondStageContinouity | rightRecoveryData.secondStageContinouity;
    gb.recovery_data.separationSwitch1 = leftRecoveryData.separationSwitch1;
    gb.recovery_data.separationSwitch2 = rightRecoveryData.separationSwitch1;
    gb.recovery_data.pressure1 = leftRecoveryData.pressure1;
    gb.recovery_data.pressure2 = rightRecoveryData.pressure1;


    return leftStatus | rightStatus;
}

static void recovery_process_data(void) {
    // Workaround - when recovery receive error occurs some data are updated with trash
    recovery_data_t copy = gb.recovery_data;

    if (recovery_read_data(&gb.recovery_data, sizeof(gb.recovery_data) - 2) == false) {
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