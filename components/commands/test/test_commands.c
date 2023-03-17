// Copyright 2022 PWrInSpace, Kuba
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


TEST_CASE("Initialization", "[CMD]") {
    command_config_t cfg = {
        .commands = commands,
        .number_of_commands = sizeof(commands) / sizeof(command_handle_t),
        .error_fnc = NULL,
    };
    TEST_ASSERT_EQUAL(true, CMD_init(&cfg));

    command_message_t cmd = {.cmd.command = 12, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(0, outputs.error);

    CMD_terminate_task();
}

TEST_CASE("Check command processing", "[CMD]") {
    command_config_t cfg = {
        .commands = commands,
        .number_of_commands = sizeof(commands) / sizeof(command_handle_t),
        .error_fnc = NULL,
    };
    TEST_ASSERT_EQUAL(true, CMD_init(&cfg));

    command_message_t cmd = CMD_create_command(TEST_COMMAND, 32);
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
    command_config_t cfg = {
        .commands = commands,
        .number_of_commands = sizeof(commands) / sizeof(command_handle_t),
        .error_fnc = on_error,
    };
    TEST_ASSERT_EQUAL(true, CMD_init(&cfg));

    command_message_t cmd = {.cmd.command = 123, .cmd.payload = 32};
    TEST_ASSERT_EQUAL(true, CMD_send_command_for_processing(&cmd));

    vTaskDelay(pdMS_TO_TICKS(25));
    TEST_ASSERT_EQUAL(COMMAND_NOT_FOUND, outputs.error);

    CMD_terminate_task();
}
