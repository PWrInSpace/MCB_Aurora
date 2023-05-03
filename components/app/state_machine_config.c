// Copyright 2022 PWrInSpace, Kuba
#include "state_machine_config.h"
#include <stddef.h>
#include "esp_log.h"
#include "sdkconfig.h"
// #include "flash_task_config.h"
#include "flash_task.h"
#include "system_timer_config.h"
#include "esp_now_config.h"
#include "commands_config.h"
#include "mission_timer.h"

#define TAG "SMC"
static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    ESP_LOGI(TAG, "ON IDLE");
}

static void on_recovery_arm(void *arg) {
    // send command to recovery
    ESP_LOGI(TAG, "ON ARM");
}

static void on_fueling(void *arg) {
    cmd_message_t cmd = cmd_create_message(MAIN_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(VENT_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    ESP_LOGI(TAG, "ON FUELING");
}

static void on_armed_to_launch(void *arg) {
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");
    FT_erase_and_run_loop();
}

static void on_ready_to_lauch(void *arg) {
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");
    sys_timer_start(TIMER_FLASH_DATA, 500, TIMER_TYPE_PERIODIC);
    // turn on camera
}

static void on_countdown(void *arg) {
    ESP_LOGI(TAG, "ON COUNTDOWN");
    if (sys_timer_stop(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to stop disconnect timer");
    }

    if (sys_timer_start(TIMER_IGNITION, 13500, TIMER_TYPE_ONE_SHOT) == false) {
        ESP_LOGE(TAG, "Unable to start ignition timer");
    }

    if (sys_timer_start(TIMER_LIFTOFF, 30000, TIMER_TYPE_ONE_SHOT) == false) {
        ESP_LOGE(TAG, "Unable to start liftoff timer");
    }

    if (mission_timer_start(30000) == false) {
        ESP_LOGE(TAG, "Unable to start mission timer");
    }

}

static void on_flight(void *arg) {
    ESP_LOGI(TAG, "ON FLIGHT");
    cmd_message_t cmd = cmd_create_message(MAIN_VALVE_OPEN, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);
}

static void on_first_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON FIRST_STAGE_RECOV");
    cmd_message_t cmd = cmd_create_message(MAIN_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);
}

static void on_second_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON SECOND_STAGE_RECOV");
    cmd_message_t cmd = cmd_create_message(VENT_VALVE_OPEN, 0x00);
    ENA_send(&esp_now_vent_valve, cmd.raw, sizeof(cmd.raw), 3);
}

static void on_ground(void *arg) {
    if (sys_timer_delete(TIMER_SD_DATA) == false) {
        ESP_LOGE(TAG, "Unable to delete sd data timer");
    }

    if (sys_timer_delete(TIMER_FLASH_DATA) == false) {
        ESP_LOGE(TAG, "Unable to delete flash data timer");
    }


    ESP_LOGI(TAG, "ON GROUND");
}


static void disable_timers_and_close_valves(void) {
    ESP_LOGI(TAG, "ON HOLD");
    if (mission_timer_is_enable() == true) {
        mission_timer_stop();
    }

    if (sys_timer_stop(TIMER_IGNITION) == false) {
        ESP_LOGE(TAG, "Unable to start ignition timer");
    }

    if (sys_timer_stop(TIMER_LIFTOFF) == false) {
        ESP_LOGE(TAG, "Unable to start liftoff timer");
    }

    cmd_message_t cmd = cmd_create_message(VENT_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(MAIN_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    // send valves close commands to tanwa
}

static void on_hold(void *arg) {
    disable_timers_and_close_valves();

    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart disconnect timer");
    }
}

static void on_abort(void *arg) {
    disable_timers_and_close_valves();

    if (sys_timer_delete(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to restart disconnect timer");
    }

    // disarm recovery module
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
