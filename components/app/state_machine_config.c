// Copyright 2022 PWrInSpace, Kuba
#include "state_machine_config.h"
#include <stddef.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "flash_task.h"
#include "system_timer_config.h"
#include "esp_now_config.h"
#include "commands_config.h"
#include "mission_timer_config.h"
#include "errors_config.h"
#include "rocket_data.h"
#include "gpio_expander.h"
#include "recovery_task_config.h"
#include "settings_mem.h"
#include "processing_task_config.h"

#define TAG "SMC"
static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    gpioexp_led_set_color(GREEN);
    cmd_message_t cmd = cmd_create_message(ETH_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valves, cmd.raw, sizeof(cmd.raw), 3);
    ESP_LOGI(TAG, "ON IDLE");
}

static void on_recovery_arm(void *arg) {
    gpioexp_led_set_color(YELLOW);
    if (recovery_send_cmd(RECOV_EASYMINI_ARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    if (recovery_send_cmd(RECOV_TELEMETRUM_ARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    ESP_LOGI(TAG, "ON ARM");
}

static void on_fueling(void *arg) {
    gpioexp_led_set_color(YELLOW);
    cmd_message_t cmd = cmd_create_message(N2O_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_ox_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2O_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(ETH_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_eth_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    ESP_LOGI(TAG, "ON FUELING");
}

static void on_pressurizing(void *arg) {
    gpioexp_led_set_color(CYAN);

    cmd_message_t cmd = cmd_create_message(N2_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    ESP_LOGI(TAG, "ON PRESSURIZING");
}

static void on_armed_to_launch(void *arg) {
    gpioexp_led_set_color(YELLOW);
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");
}

static void on_ready_to_lauch(void *arg) {

    gpioexp_led_set_color(PURPLE);
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");
    Settings settings = settings_get_all();

    gpioexp_camera_turn_on();
    if (settings.flash_on != 0) {
        FT_start_loop();
        sys_timer_start(TIMER_FLASH_DATA, 500, TIMER_TYPE_PERIODIC);
    }
}

static void on_countdown(void *arg) {

    ESP_LOGI(TAG, "ON COUNTDOWN");

    if (sys_timer_stop(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to stop disconnect timer");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_MISSION_TIMER, 100);
        goto abort_countdown;
    }


    Settings settings = settings_get_all();
    if (liquid_mission_timer_start(settings.countdownTime, settings.ignitTime) == false) {
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

static void recovery_first_stage_process(recovery_data_t *data) {
    if (data == NULL) {
        return;
    }

    if (data->firstStageDone == true) {
        if (SM_change_state(FIRST_STAGE_RECOVERY) != SM_OK) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 1000);
        }
    }
}

static void lift_off_process(void *data_buffer) {

    static uint8_t liftoff_counter = 0;
    sensors_data_t *data = (sensors_data_t *)data_buffer;

    if (data->altitude > 10.0f) {
        liftoff_counter += 1;
    } else {
        liftoff_counter = 0;
    }
    if (liftoff_counter >= 2) {
        if (SM_change_state(BURN) != SM_OK) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 1000);
        }
    }
}

static void on_lift_off(void *arg) {
    ESP_LOGI(TAG, "----> ON LIFT_OFF <----");
    if (recovery_change_process_fnc(recovery_first_stage_process) == false) {
        ESP_LOGE(TAG, "Unable do add process fnc");
    }

    if (sensors_change_process_function(lift_off_process, 100) == false) {
        ESP_LOGE(TAG, "Unable to add process function");
    }

    cmd_message_t cmd = cmd_create_message(VALVE_DZIDA, 0x00);
    ENA_send(&esp_now_broadcast, cmd.raw, sizeof(cmd.raw), 5);

}

static void burn_process(void *data_buffer) {
    
    static uint8_t burn_counter = 0;
    sensors_data_t *data = (sensors_data_t *)data_buffer;

    if (data->altitude > 50.0f && data->acc_vertical < -8.5f) {
        burn_counter += 1;
    } else {
        burn_counter = 0;
    }

    if (burn_counter >= 5) {
        if (SM_change_state(FLIGHT) != SM_OK) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 1000);
        }
    }
}

static void on_burn(void *arg) {
    ESP_LOGI(TAG, "----> ON BURN <----");
    if (sensors_change_process_function(burn_process, 100) == false) {
        ESP_LOGE(TAG, "Unable to add process function");
    }
}

static void on_flight(void *arg) {
    ESP_LOGI(TAG, "----> ON FLIGHT <----");

    sensors_remove_process_function(1000);
    gpioexp_led_set_color(RED);
}

static void recovery_second_stage_process(recovery_data_t *data) {
    if (data == NULL) {
        return;
    }

    if (data->secondStageDone == true) {
        if (SM_change_state(SECOND_STAGE_RECOVERY) != SM_OK) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 1000);
        }
    }
}

static void on_first_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON FIRST_STAGE_RECOV");
    if (recovery_change_process_fnc(recovery_second_stage_process) == false) {
        ESP_LOGE(TAG, "Unable do add process fnc");
    }

    if (recovery_send_cmd(RECOV_FORCE_FIRST_STAGE, 0) == false) {
        ESP_LOGE(TAG, "Unable to send first stage recov");
    }
}

static void on_ground_sensors_process(void *data_buffer) {
    static uint8_t ground_counter = 0;
    sensors_data_t *data = (sensors_data_t*) data_buffer;

    if (data->altitude < 50) {
        ground_counter += 1;
    } else {
        ground_counter = 0;
    }

    if (ground_counter >= 5) {
        if (SM_change_state(ON_GROUND) != SM_OK) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 1000);
        }
    }
}

static void on_second_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON SECOND_STAGE_RECOV");
    if (recovery_remove_process_fnc() == false) {
        ESP_LOGE(TAG, "Unable do remove process fnc");
    }

    if (sensors_change_process_function(on_ground_sensors_process, 1000) == false) {
        ESP_LOGE(TAG, "Unable to add process function");
    }

    if (recovery_send_cmd(RECOV_FORCE_SECOND_STAGE, 0) == false) {
        ESP_LOGE(TAG, "Unable to send first stage recov");
    }

    cmd_message_t cmd = cmd_create_message(ETH_SOL_OPEN, 0x00);
    ENA_send(&esp_now_eth_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2O_SOL_OPEN, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_SOL_OPEN, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);
}

static void on_ground(void *arg) {
    if (sensors_remove_process_function(1000) == false) {
        ESP_LOGE(TAG, "Unable to remove process fnc");
    }

    if (sys_timer_delete(TIMER_SD_DATA) == false) {
        ESP_LOGE(TAG, "Unable to delete sd data timer");
    }

    if (sys_timer_delete(TIMER_FLASH_DATA) == false) {
        ESP_LOGE(TAG, "Unable to delete flash data timer");
    }

    gpioexp_camera_turn_off();
    ESP_LOGI(TAG, "ON GROUND");
    gpioexp_led_set_color(CYAN);
}


static void close_valves(void) {

    cmd_message_t cmd = cmd_create_message(N2O_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_ox_main_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(ETH_VALVE_CLOSE, 0x00);
    ENA_send(&esp_now_main_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2O_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(ETH_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_eth_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

}

static void close_valves_on_lift_off(void) {

    cmd_message_t cmd = cmd_create_message(ETH_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_eth_vent_valve, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2O_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

    cmd = cmd_create_message(N2_SOL_CLOSE, 0x00);
    ENA_send(&esp_now_vent_valves, cmd.raw, sizeof(cmd.raw), 3);

}

static void on_hold(void *arg) {
    ESP_LOGI(TAG, "ON HOLD");

    if (liquid_mission_timer_stop() == false) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_MISSION_TIMER, 100);
        ESP_LOGE(TAG, "Unable to stop mission timer");
    }

    if (sys_timer_stop(TIMER_FLASH_DATA) == false) {
        ESP_LOGE(TAG, "Unable to stop flash data timer");
    }
    
    close_valves();

    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart disconnect timer");
    }

    gpioexp_led_set_color(BLUE);
}

static void on_abort(void *arg) {
    ESP_LOGI(TAG, "ON ABORT");

    state_id current_state = SM_get_current_state();

    if (liquid_mission_timer_stop() == false) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_MISSION_TIMER, 100);
        ESP_LOGE(TAG, "Unable to stop mission timer");
    }

    if (sys_timer_stop(TIMER_FLASH_DATA) == false) {
        ESP_LOGE(TAG, "Unable to stop flash data timer");
    }

    if(current_state == LIFT_OFF) {
        ESP_LOGI(TAG, "Abort during LIFT_OFF");

        //in case of not lifting off close vents
        close_valves_on_lift_off();
    }
    else {

        //in case of general abort close all valves        
        close_valves();
    }

    if (sys_timer_delete(TIMER_DISCONNECT) == false) {
        ESP_LOGE(TAG, "Unable to delete disconnect timer");
    }

    // disarm recovery module
    if (recovery_send_cmd(RECOV_EASYMINI_DISARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    if (recovery_send_cmd(RECOV_TELEMETRUM_DISARM, 0x00) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }


    gpioexp_led_set_color(NONE);
}

static state_config_t states_cfg[] = {
    {INIT, on_init, NULL},
    {IDLE, on_idle, NULL},
    {RECOVERY_ARM, on_recovery_arm, NULL},
    {FUELING, on_fueling, NULL},
    {PRESSURIZING, on_pressurizing, NULL},
    {ARMED_TO_LAUNCH, on_armed_to_launch, NULL},
    {RDY_TO_LAUNCH, on_ready_to_lauch, NULL},
    {COUNTDOWN, on_countdown, NULL},
    {LIFT_OFF, on_lift_off, NULL},
    {BURN, on_burn, NULL},
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
