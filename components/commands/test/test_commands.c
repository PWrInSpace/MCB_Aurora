// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "unity.h"
#include "commands.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static struct {
    int32_t test_payload;
    int32_t second_test_payload;
    COMMAND_ERROR error;
} outputs = {
    .test_payload = 0,
    .second_test_payload = 0,
    .error = 0
};

static void clear_outputs(void) {
    memset(&outputs, 0, sizeof(outputs));
}

static void on_test_command(uint32_t command, int32_t payload) {
    outputs.test_payload = payload;
}

static void on_second_command(uint32_t command, int32_t payload) {
    outputs.second_test_payload = payload;
}

static void on_error(COMMAND_ERROR error) {
    outputs.error = error;
}

typedef enum {
    TEST_COMMAND,
    SECOND_COMMAND
} COMMAND_NAME;

command_handle_t commands[] = {
    {TEST_COMMAND, on_test_command},
    {SECOND_COMMAND, on_second_command},
};

static void initialzie_commands_config(command_config_t *cfg) {
    cfg->commands = commands;
    cfg->number_of_commands = sizeof(commands) / sizeof(command_handle_t);
    cfg->task_error_fnc = NULL;
    cfg->queue_size = 10;
    cfg->stack_depth = 2048;
    cfg->priority = 2;
    cfg->core_id = 1;
}


TEST_CASE("Init with task", "[CMD]") {
    command_config_t cfg;
    initialzie_commands_config(&cfg);
    TEST_ASSERT_EQUAL(true, CMD_init_with_task(&cfg));

    command_message_t cmd = {.cmd.command = 12, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(0, outputs.error);

    CMD_terminate_task();
}

TEST_CASE("Check command processing", "[CMD]") {
    command_config_t cfg;
    initialzie_commands_config(&cfg);
    TEST_ASSERT_EQUAL(true, CMD_init_with_task(&cfg));

    command_message_t cmd = CMD_create_message(TEST_COMMAND, 32);
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(32, outputs.test_payload);

    cmd.cmd.command = SECOND_COMMAND;
    cmd.cmd.payload = 122;
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(122, outputs.second_test_payload);

    CMD_terminate_task();
}

TEST_CASE("Check error", "[CMD]") {
    command_config_t cfg;
    initialzie_commands_config(&cfg);
    cfg.task_error_fnc = on_error;
    TEST_ASSERT_EQUAL(true, CMD_init_with_task(&cfg));

    command_message_t cmd = {.cmd.command = 123, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(COMMAND_NOT_FOUND, outputs.error);

    CMD_terminate_task();
}

TEST_CASE("Init without task", "[CMD]") {
    command_config_t cfg;
    initialzie_commands_config(&cfg);
    TEST_ASSERT_EQUAL(true, CMD_init(&cfg));

    command_message_t cmd = {.cmd.command = 123, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(false, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(COMMAND_NOT_FOUND, outputs.error);
    CMD_terminate_task();
}


TEST_CASE("Process unknown message without task", "[CMD]") {
    command_config_t cfg;
    initialzie_commands_config(&cfg);
    TEST_ASSERT_EQUAL(true, CMD_init(&cfg));

    command_message_t cmd = {.cmd.command = 123, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(false, CMD_process_command(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
}

TEST_CASE("Process unknown message without task", "[CMD]") {
    command_message_t cmd = {.cmd.command = TEST_COMMAND, .cmd.payload = 32};
    clear_outputs();
    TEST_ASSERT_EQUAL(true, CMD_process_command(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(32, outputs.test_payload);
}
