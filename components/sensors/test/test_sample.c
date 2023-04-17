// Copyright 2022 PWrInSpace, Kuba
#include "unity.h"
#include "processing_task.h"

typedef struct {
    uint32_t a;
    int b;
    float c;
} sensors_test_t;

static void sensors_read_data(void *data_buffer) {
    return;
}

static void sensors_process_data(void *data_buffer) {
    return;
}

TEST_CASE("Initialize with 0 data size", "[PROCESSING]") {
    sensors_task_cfg_t cfg = {
        .sensors_process_fnc = sensors_process_data,
        .sensors_read_fnc = sensors_read_data,
        .data_size = 0,
    };

    TEST_ASSERT_EQUAL(false, sensors_create_task(&cfg));
}

TEST_CASE("Get data with uninitialize lib", "[PROCESSING]") {
    sensors_test_t test;

    TEST_ASSERT_EQUAL(false, sensors_get_data(&test, sizeof(test), 1000));
}

TEST_CASE("Inject processing fnc with uninitialize lib", "[PROCESSING]") {
    TEST_ASSERT_EQUAL(false, sensors_change_process_function(sensors_process_data, 1000));
}

TEST_CASE("Initialize without read data fnc", "[PROCESSING]") {
    sensors_task_cfg_t cfg = {
        .sensors_process_fnc = sensors_process_data,
        .sensors_read_fnc = NULL,
        .data_size = sizeof(sensors_test_t),
    };

    TEST_ASSERT_EQUAL(false, sensors_create_task(&cfg));
}
// TASK IS RUNNNIG
TEST_CASE("Initialize without process fnc", "[PROCESSING]") {
    sensors_task_cfg_t cfg = {
        .sensors_process_fnc = NULL,
        .sensors_read_fnc = sensors_read_data,
        .data_size = sizeof(sensors_test_t),
    };

    TEST_ASSERT_EQUAL(true, sensors_create_task(&cfg));
}

TEST_CASE("Inject process fnc", "[PROCESSING]") {
    TEST_ASSERT_EQUAL(true, sensors_change_process_function(sensors_process_data, 100));
}

TEST_CASE("Get data with diffrent size", "[PROCESSING]") {
    sensors_test_t test;
    TEST_ASSERT_EQUAL(false, sensors_get_data(&test, sizeof(test) - 2, 100));
}