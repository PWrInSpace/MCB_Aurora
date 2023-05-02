// Copyright 2022 PWrInSpace, Kuba
#include "state_machine_config.h"
#include <stddef.h>
#include "esp_log.h"
#include "sdkconfig.h"
// #include "flash_task_config.h"
#include "flash_task.h"
#include "system_timer_config.h"

#define TAG "SMC"
static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    FT_erase_and_run_loop();
    ESP_LOGI(TAG, "ON IDLE");
}

static void on_recovery_arm(void *arg) {
    sys_timer_start(TIMER_FLASH_DATA, 500, TIMER_TYPE_PERIODIC);
    ESP_LOGI(TAG, "ON ARM");
}

static void on_fueling(void *arg) {
    ESP_LOGI(TAG, "ON FUELING");
}

static void on_armed_to_launch(void *arg) {
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");
}

static void on_ready_to_lauch(void *arg) {
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");
}

static void on_countdown(void *arg) {
    ESP_LOGI(TAG, "ON COUNTDOWN");
}

static void on_flight(void *arg) {
    ESP_LOGI(TAG, "ON FLIGHT");
}

static void on_first_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON FIRST_STAGE_RECOV");
}

static void on_second_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON SECOND_STAGE_RECOV");
}

static void on_ground(void *arg) {
    ESP_LOGI(TAG, "ON GROUND");
}

static void on_hold(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_abort(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static state_config_t states_cfg[] = {
    {INIT, on_init, NULL},
    {IDLE, on_idle, NULL},
    {RECOVERY_ARM, on_recovery_arm, NULL},
    {FUELING, on_fueling, NULL},
    {ARMED_TO_LAUNCH, on_armed_to_launch, NULL},
    {RDY_TO_LAUNCH, on_ready_to_lauch, NULL},
    {COUNTDOWN, on_countdown, NULL},
    {FLIGHT, on_flight, NULL},
    {FIRST_STAGE_RECOVERY, on_first_stage_recovery, NULL},
    {SECOND_STAGE_RECOVERY, on_second_stage_recovery, NULL},
    {ON_GROUND, on_ground, NULL},
    {HOLD, on_hold, NULL},
    {ABORT, on_abort, NULL},
};

bool initialize_state_machine(void) {
    ESP_LOGI(TAG, "Initializing state machine");
    state_machine_task_cfg_t task_cfg = {
        .stack_depth = CONFIG_SM_TASK_STACK_DEPTH,
        .core_id = CONFIG_SM_TASK_CORE_ID,
        .priority = CONFIG_SM_TASK_PRIORITY,
    };

    SM_Response status = SM_OK;
    SM_init();
    status |= SM_set_states(states_cfg, sizeof(states_cfg)/sizeof(states_cfg[0]));
    status |= SM_run(&task_cfg);
    return status == SM_OK ? true : false;
}
