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
#include "mission_timer_config.h"
#include "errors_config.h"
#include "rocket_data.h"
#include "gpio_expander.h"
#include "recovery_task_config.h"

#define TAG "SMC"
static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    gpioexp_led_set_color(GREEN);
    ESP_LOGI(TAG, "ON IDLE");
}

static void on_recovery_arm(void *arg) {
    gpioexp_led_set_color(YELLOW);
    if (recovery_send_cmd(RECOV_EASYMINI_ARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    if (recovery_send_cmd(RECOV_TELEMETRUM_ARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    ESP_LOGI(TAG, "ON ARM");
}

static void on_fueling(void *arg) {
    gpioexp_led_set_color(YELLOW);
    cmd_message_t cmd = cmd_create_message(MAIN_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(VENT_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    ESP_LOGI(TAG, "ON FUELING");
}

static void on_armed_to_launch(void *arg) {
    gpioexp_led_set_color(YELLOW);
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");
    FT_erase_and_run_loop();
}

static void on_ready_to_lauch(void *arg) {
    gpioexp_led_set_color(PURPLE);
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");
    sys_timer_start(TIMER_FLASH_DATA, 500, TIMER_TYPE_PERIODIC);
    // turn on camera
}

static void on_countdown(void *arg) {
    ESP_LOGI(TAG, "ON COUNTDOWN");

    if (sys_timer_stop(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to stop disconnect timer");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_MISSION_TIMER, 100);
        goto abort_countdown;
    }

    if (hybrid_mission_timer_start(-30000, -13500) == false) {
        ESP_LOGE(TAG, "Mission timer error");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_DISCONNECT_TIMER, 100);
        goto abort_countdown;
    }

    gpioexp_led_set_color(RED);
    return;

abort_countdown:
    SM_change_to_previous_state(true);
    sys_timer_start(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS, TIMER_TYPE_ONE_SHOT);
}

static void on_flight(void *arg) {
    ESP_LOGI(TAG, "----> ON FLIGHT <----");
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
    gpioexp_led_set_color(CYAN);
}


static void disable_timers_and_close_valves(void) {
    if (hybrid_mission_timer_stop() == false) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_MISSION_TIMER, 100);
        ESP_LOGE(TAG, "Unable to stop mission timer");
    }

    if (sys_timer_stop(TIMER_FLASH_DATA) == false) {
        ESP_LOGE(TAG, "Unable to stop flash data timer");
    }

    cmd_message_t cmd = cmd_create_message(VENT_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(MAIN_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    // send valves close commands to tanwa
}

static void on_hold(void *arg) {
    ESP_LOGI(TAG, "ON HOLD");
    disable_timers_and_close_valves();

    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart disconnect timer");
    }

    gpioexp_led_set_color(BLUE);
}

static void on_abort(void *arg) {
    ESP_LOGI(TAG, "ON ABORT");
    disable_timers_and_close_valves();

    if (sys_timer_delete(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to delete disconnect timer");
    }

    // disarm recovery module
    if (recovery_send_cmd(RECOV_EASYMINI_DISARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    if (recovery_send_cmd(RECOV_TELEMETRUM_DISARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }


    gpioexp_led_set_color(WHITE);
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
