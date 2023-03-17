// Copyright 2022 PWrInSpace, Kuba

#include <string.h>
#include "commands.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "CMD"

static struct {
    TaskHandle_t task;
    QueueHandle_t receive_queue;

    command_handle_t *registered_commands;
    size_t number_of_commands;

    cmd_on_error error_fnc;
} gb = {
    .task = NULL,
    .receive_queue = NULL,
    .registered_commands = NULL,
    .number_of_commands = 0,
    .error_fnc = NULL,
};

static void report_error(COMMAND_ERROR error) {
    ESP_LOGE(TAG, "ERROR %d", error);
    if (gb.error_fnc != NULL) {
        gb.error_fnc(error);
    }
}

static void find_command_and_execute(command_message_t *received_command) {
    for (size_t i = 0; i < gb.number_of_commands; ++i) {
        if (gb.registered_commands[i].command == received_command->cmd.command) {
            if (gb.registered_commands[i].receive_fnc != NULL) {
                gb.registered_commands[i].receive_fnc(
                        received_command->cmd.command, received_command->cmd.payload);
            }
            return;
        }
    }

    report_error(COMMAND_NOT_FOUND);
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

bool CMD_init(command_config_t *cfg) {
    assert(gb.task != NULL);
    if (gb.task != NULL) {
        return false;
    }

    assert(cfg->commands != NULL);
    assert(cfg->number_of_commands > 0);
    if (cfg->commands == NULL || cfg->number_of_commands == 0) {
        return false;
    }

    gb.registered_commands = cfg->commands;
    gb.number_of_commands = cfg->number_of_commands;
    gb.error_fnc = cfg->error_fnc;

    gb.receive_queue = xQueueCreate(CMD_RECEIVE_QUEUE_SIZE, sizeof(command_message_t));
    if (gb.receive_queue == NULL) {
        return false;
    }

    xTaskCreatePinnedToCore(
        command_task,
        "Command task",
        CMD_TASK_STACK_DEPTH,
        NULL,
        CMD_TASK_PRIORITY,
        &gb.task,
        CMD_TASK_CORE_ID);

    if (gb.task == NULL) {
        vQueueDelete(gb.receive_queue);
        gb.receive_queue = NULL;
        return false;
    }

    return true;
}

bool CMD_send_command_for_processing(command_message_t *command) {
    assert(command != NULL);
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

command_message_t CMD_create_command(uint32_t command, int32_t payload) {
    command_message_t message = {
        .cmd.command = command,
        .cmd.payload = payload
    };

    return message;
}

void CMD_terminate_task(void) {
    vQueueDelete(gb.receive_queue);
    vTaskDelete(gb.task);
    memset(&gb, 0, sizeof(gb));
    gb.receive_queue = NULL;
}

