// Copyright 2022 PWrInSpace, Kuba

#include <string.h>
#include "commands.h"
#include "esp_log.h"

#define TAG "CMD"

static struct {
    TaskHandle_t task;
    QueueHandle_t receive_queue;

    command_handle_t *registered_commands;
    size_t number_of_commands;

    cmd_on_task_error error_task_fnc;
} gb = {
    .task = NULL,
    .receive_queue = NULL,
    .registered_commands = NULL,
    .number_of_commands = 0,
    .error_task_fnc = NULL,
};

static void report_error(COMMAND_ERROR error) {
    ESP_LOGE(TAG, "ERROR %d", error);
    if (gb.error_task_fnc != NULL) {
        gb.error_task_fnc(error);
    }
}

static bool find_command_and_execute(command_message_t *received_command) {
    for (size_t i = 0; i < gb.number_of_commands; ++i) {
        if (gb.registered_commands[i].command == received_command->cmd.command) {
            if (gb.registered_commands[i].receive_fnc != NULL) {
                gb.registered_commands[i].receive_fnc(
                        received_command->cmd.command, received_command->cmd.payload);
            }
            return true;
        }
    }

    if (gb.task != NULL) {
        report_error(COMMAND_NOT_FOUND);
    }

    return false;
}

static void command_task(void *arg) {
    command_message_t received_command;
    while (1) {
        if (xQueueReceive(gb.receive_queue, &received_command, 0) == pdTRUE) {
            if (gb.registered_commands != NULL) {
                find_command_and_execute(&received_command);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


static bool register_commands_and_err_fnc(command_config_t *cfg) {
    assert(cfg->commands != NULL);
    assert(cfg->number_of_commands > 0);
    if (cfg->commands == NULL || cfg->number_of_commands == 0) {
        return false;
    }

    gb.registered_commands = cfg->commands;
    gb.number_of_commands = cfg->number_of_commands;
    gb.error_task_fnc = cfg->task_error_fnc;

    return true;
}

bool CMD_init(command_config_t *cfg) {
    return register_commands_and_err_fnc(cfg);
}

bool CMD_init_with_task(command_config_t *cfg) {
    if (gb.task != NULL) {
        return false;
    }

    if (register_commands_and_err_fnc(cfg) == false) {
        return false;
    }

    gb.receive_queue = xQueueCreate(cfg->queue_size, sizeof(command_message_t));
    if (gb.receive_queue == NULL) {
        return false;
    }

    xTaskCreatePinnedToCore(
        command_task,
        "Command task",
        cfg->stack_depth,
        NULL,
        cfg->priority,
        &gb.task,
        cfg->core_id);

    if (gb.task == NULL) {
        vQueueDelete(gb.receive_queue);
        gb.receive_queue = NULL;
        return false;
    }

    return true;
}

bool CMD_send_command_for_processing(command_message_t *command) {
    assert(command != NULL);
    assert(gb.receive_queue != NULL);
    if (command == NULL) {
        return false;
    }

    if (gb.receive_queue == NULL) {
        return false;
    }

    if (xQueueSend(gb.receive_queue, command, 0) != pdTRUE) {
        ESP_LOGE(TAG, "Unable to add to queue");
        return false;
    }

    return true;
}

bool CMD_process_command(command_message_t *command) {
    return find_command_and_execute(command);
}

command_message_t CMD_create_message(uint32_t command, int32_t payload) {
    command_message_t message = {
        .cmd.command = command,
        .cmd.payload = payload
    };

    return message;
}

void CMD_terminate_task(void) {
    if (gb.receive_queue != NULL) {
        vQueueDelete(gb.receive_queue);
        gb.receive_queue = NULL;
    }

    if (gb.task != NULL) {
        vTaskDelete(gb.task);
        gb.task = NULL;
    }
}

