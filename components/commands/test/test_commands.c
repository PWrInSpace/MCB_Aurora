// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "unity.h"
#include "commands.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "TEST"

static struct {
    int32_t payload_valve_open;
    int32_t payload_valve_close;
    int32_t payload_pitot_one;
    int32_t payload_pitot_two;
} outputs = {
    .payload_valve_open = 0,
    .payload_valve_close = 0,
    .payload_pitot_one = 0,
    .payload_pitot_two = 0,
};

#define PRIVILAGE_OUTPUT 332

static void on_valve_open_command(uint32_t command, int32_t payload, bool privilage) {
    outputs.payload_valve_open = payload;
}

static void on_valve_close_command(uint32_t command, int32_t payload, bool privilage) {
    outputs.payload_valve_close = payload;
}

static void on_pitot_one_command(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "ONE COMMAND");
    outputs.payload_pitot_one = payload;
}

static void on_pitot_two_command(uint32_t command, int32_t payload, bool privilage) {
    if (privilage == true) {
        outputs.payload_pitot_two = PRIVILAGE_OUTPUT;
    } else {
        outputs.payload_pitot_two = payload;
    }
}

static void clear_outputs(void) {
    memset(&outputs, 0, sizeof(outputs));
}

#define LORA_DEV_ID 0x02

typedef enum {
    VALVE,
    PITOT,
} devices_t;

typedef enum {
    VALVE_OPEN = 12,
    VALVE_CLOSE
} valve_commands_t;

rf_cmd_command_t valve_commands[] = {
    {VALVE_OPEN, on_valve_open_command},
    {VALVE_CLOSE, on_valve_close_command}
};

typedef enum {
    PITOT_ONE,
    PITOT_TWO
} pitot_commands_t;

rf_cmd_command_t pitot_commands[] = {
    {PITOT_ONE, on_pitot_one_command},
    {PITOT_TWO, on_pitot_two_command}
};

rf_cmd_device_t devices[] = {
    {VALVE, valve_commands, sizeof(valve_commands) / sizeof(rf_cmd_command_t)},
    {PITOT, pitot_commands, sizeof(pitot_commands) / sizeof(rf_cmd_command_t)},
};


TEST_CASE("Init in standard mode", "[RF_CMD]") {
    rf_cmd_config_t cfg = {
        .devices = devices,
        .number_of_devices = sizeof(devices) / sizeof(rf_cmd_device_t)
    };
    TEST_ASSERT_EQUAL(true, rf_cmd_init_standard_mode(&cfg));
}

TEST_CASE("Try prcoess lora in standard mode", "[RF_CMD]") {
    rf_cmd_message_t mess = rf_cmd_create_message(PITOT_ONE, 123);
    TEST_ASSERT_EQUAL(false, rf_cmd_process_lora_command(0x12, PITOT, &mess));
}

TEST_CASE("Process commands with invalid dev", "[RF_CMD]") {
    rf_cmd_message_t mess = rf_cmd_create_message(PITOT_ONE, 123);
    TEST_ASSERT_EQUAL(false, rf_cmd_process_command(123, &mess));
}

TEST_CASE("Process commands in normal mode", "[RF_CMD]") {
    rf_cmd_message_t mess = rf_cmd_create_message(PITOT_ONE, 123);
    TEST_ASSERT_EQUAL(true, rf_cmd_process_command(PITOT, &mess));
    TEST_ASSERT_EQUAL(123, outputs.payload_pitot_one);
    mess = rf_cmd_create_message(VALVE_OPEN, 152);
    TEST_ASSERT_EQUAL(true, rf_cmd_process_command(VALVE, &mess));
    TEST_ASSERT_EQUAL(152, outputs.payload_valve_open);
}

TEST_CASE("Init in lora mode", "[RF_CMD]") {
    rf_cmd_lora_config_t cfg = {
        .lora_dev_id = LORA_DEV_ID,
        .devices = devices,
        .number_of_devices = sizeof(devices) / sizeof(rf_cmd_device_t)
    };
    TEST_ASSERT_EQUAL(true, rf_cmd_init_lora_mode(&cfg));
}


TEST_CASE("Process commands in lora mode", "[RF_CMD]") {
    clear_outputs();

    rf_cmd_message_t mess = rf_cmd_create_message(PITOT_ONE, 123);
    TEST_ASSERT_EQUAL(true, rf_cmd_process_lora_command(LORA_DEV_ID, PITOT, &mess));
    TEST_ASSERT_EQUAL(123, outputs.payload_pitot_one);
    mess = rf_cmd_create_message(VALVE_OPEN, 152);
    TEST_ASSERT_EQUAL(false, rf_cmd_process_lora_command(LORA_DEV_ID + 0x22, VALVE, &mess));
}

TEST_CASE("Check privilage mode", "[RF_CMD]") {
    clear_outputs();

    rf_cmd_message_t mess = rf_cmd_create_message(PITOT_TWO, 123);
    TEST_ASSERT_EQUAL(true, rf_cmd_process_lora_command(
        LORA_DEV_ID | RF_CMD_PRIVILAGE_MASK,
        PITOT,
        &mess));
    TEST_ASSERT_EQUAL(PRIVILAGE_OUTPUT, outputs.payload_pitot_two);
}