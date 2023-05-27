#include "recovery_task_config.h"
#include "basic_task.h"
#include "commands.h"
#include "rocket_data.h"
#include "errors_config.h"
#include "esp_log.h"
#include "i2c.h"

#define TAG "RECOVERY"

static struct {
    basic_task_t task;
    recovery_data_t recovery_data;
} gb;

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
    if (recovery_read_data((uint8_t*)&gb.recovery_data, sizeof(gb.recovery_data)) == false) {
        ESP_LOGE(TAG, "RECOVERY RECEIVE ERROR");
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_RECEIVE, 100);
    }

    ESP_LOGI(TAG, "Pressure1 %d", gb.recovery_data.pressure1);
    rocket_data_update_recovery(&gb.recovery_data);
}


bool initialize_recovery(void) {
    basic_task_cfg_t task_cfg = {
        .process_fnc = recovery_process_data,
        .priority = CONFIG_GPS_TASK_PRIORITY,
        .core_id = CONFIG_GPS_TASK_CPU,
        .stack_depth = CONFIG_GPS_TASK_DEPTH,
        .task_delay = CONFIG_GPS_TASK_PERIOD_MS,
    };

    if (basic_task_create(&task_cfg, &gb.task) == false) {
        return false;
    }

    return true;
}