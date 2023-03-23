// Copyright 2022 PWrInSpace, Kuba

#include <string.h>
#include "commands.h"
#include "esp_log.h"

#define TAG "CMD"

static struct {
    // TaskHandle_t task;
    // QueueHandle_t receive_queue;

    rf_cmd_device_t *devices;
    size_t number_of_devices;

    rf_cmd_on_task_error error_task_fnc;
} gb = {
    // .task = NULL,
    // .receive_queue = NULL,
    .devices = NULL,
    .number_of_devices = 0,
    .error_task_fnc = NULL,
};

static void report_error(COMMAND_ERROR error) {
    ESP_LOGE(TAG, "ERROR %d", error);
    if (gb.error_task_fnc != NULL) {
        gb.error_task_fnc(error);
    }
}


static bool get_device_index(rf_cmd_device_id_t dev_id, size_t *dev_index) {
    for (size_t i = 0; gb.number_of_devices; ++i) {
        if (gb.devices[i].dev_id == dev_id) {
            *dev_index = i;
            return true;
        }
    }
    return false;
}

static bool find_command_and_execute(size_t dev_index, rf_cmd_message_t *received_command) {
    for (size_t i = 0; i < gb.devices[dev_index].number_of_commands; ++i) {
        if (gb.devices->commands[i].command_id == received_command->cmd.command) {
            if (gb.devices->commands[i].on_command_receive_fnc != NULL) {
                gb.devices->commands[i].on_command_receive_fnc(
                        received_command->cmd.command, received_command->cmd.payload);
            }
            return true;
        }
    }

    // if (gb.task != NULL) {
    //     report_error(COMMAND_NOT_FOUND);
    // }

    return false;
}

// static void command_task(void *arg) {
//     rf_cmd_message_t received_command;
//     while (1) {
//         if (xQueueReceive(gb.receive_queue, &received_command, 0) == pdTRUE) {
//             if (gb.registered_commands != NULL) {
//                 find_command_and_execute(&received_command);
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }


static bool register_commands_and_err_fnc(rf_cmd_config_t *cfg) {
    assert(cfg->devices != NULL);
    assert(cfg->number_of_devices > 0);
    if (cfg->devices == NULL || cfg->number_of_devices == 0) {
        return false;
    }

    gb.devices = cfg->devices;
    gb.number_of_devices = cfg->number_of_devices;
    gb.error_task_fnc = cfg->task_error_fnc;

    return true;
}

bool rf_cmd_init(rf_cmd_config_t *cfg) {
    return register_commands_and_err_fnc(cfg);
}

// bool rf_cmd_init_with_task(rf_cmd_config_t *cfg) {
//     if (gb.task != NULL) {
//         return false;
//     }

//     if (register_commands_and_err_fnc(cfg) == false) {
//         return false;
//     }

//     gb.receive_queue = xQueueCreate(cfg->queue_size, sizeof(rf_cmd_message_t));
//     if (gb.receive_queue == NULL) {
//         return false;
//     }

//     xTaskCreatePinnedToCore(
//         command_task,
//         "Command task",
//         cfg->stack_depth,
//         NULL,
//         cfg->priority,
//         &gb.task,
//         cfg->core_id);

//     if (gb.task == NULL) {
//         vQueueDelete(gb.receive_queue);
//         gb.receive_queue = NULL;
//         return false;
//     }

//     return true;
// }

// bool rf_cmd_send_command_for_processing(rf_cmd_message_t *command) {
//     assert(command != NULL);
//     assert(gb.receive_queue != NULL);
//     if (command == NULL) {
//         return false;
//     }

//     if (gb.receive_queue == NULL) {
//         return false;
//     }

//     if (xQueueSend(gb.receive_queue, command, 0) != pdTRUE) {
//         ESP_LOGE(TAG, "Unable to add to queue");
//         return false;
//     }

//     return true;
// }

bool rf_cmd_process_command(rf_cmd_device_id_t dev_id, rf_cmd_message_t *command) {
    size_t index = 0;
    if (get_device_index(dev_id, &index) == false) {
        return false;
    }
    return find_command_and_execute(index ,command);
}

rf_cmd_message_t rf_cmd_create_message(uint32_t command, int32_t payload) {
    rf_cmd_message_t message = {
        .cmd.command = command,
        .cmd.payload = payload
    };

    return message;
}

// void rf_cmd_terminate_task(void) {
//     if (gb.receive_queue != NULL) {
//         vQueueDelete(gb.receive_queue);
//         gb.receive_queue = NULL;
//     }

//     if (gb.task != NULL) {
//         vTaskDelete(gb.task);
//         gb.task = NULL;
//     }
// }

