// Copyright 2022 PWrInSpace, Kuba

#include "commands_config.h"
#include "esp_log.h"
#include "state_machine_config.h"
#include "lora_task_config.h"
#include "esp_now_config.h"
#include "errors_config.h"
#include "rocket_data.h"
#include "recovery_task_config.h"
#include "system_timer_config.h"
#include "settings_mem.h"
#include "mission_timer_config.h"
#include "flash_task.h"
#include "buzzer_pwm.h"
#include "gpio_expander.h"

#define TAG "CMD"
// COMMANDS
// https://docs.google.com/spreadsheets/d/1kvS3BirYGmhAizmF42UDyF-SwWfSJDnYGSNufSPmQ5g/edit#gid=1424247026

// MCB
static bool state_change_check_countdown(void) {
    recovery_data_t data = rocket_data_get_recovery();
    tanwa_data_t tanwa = rocket_data_get_tanwa();
    if (data.isArmed == false || data.isTeleActive == false || tanwa.soft_arm == false) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_NOT_ARMED, 100);
        return false;
    }

    // if (rocket_data_woken_up() == false) {
    //     ESP_LOGE(TAG, "On or more devices are sleeping");
    //     errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_WAKE_UP, 100);
    //     return false;
    // }

    return true;
}

static void send_command_esp_now(const ENA_device_t *dev, uint32_t command, int32_t payload) {
    cmd_message_t mess = cmd_create_message(command, payload);
    if (ENA_send(dev, mess.raw, sizeof(mess), 3) != ESP_OK) {
        ESP_LOGI(TAG, "Unable to send command %d, %d", command, payload);
    }
}

static void tanwa_process_command(uint32_t command, int32_t payload, bool privilege) {
    send_command_esp_now(&esp_now_tanwa, command, payload);
}

static void mcb_state_change(uint32_t command, int32_t payload, bool privilege) {
    ESP_LOGI(TAG, "Changing state to -> %d", payload);

    if (payload == COUNTDOWN && privilege == false) {
        if (state_change_check_countdown() == false) {
            return;
        }
    }

    if (SM_force_change_state(payload) != SM_OK) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 100);
        ESP_LOGE(TAG, "Unable to change state");
        return;
    }

    ESP_LOGI(TAG, "STATE CHANGED");
}

static void mcb_abort(uint32_t command, int32_t payload, bool privilege) {
    states_t state = SM_get_current_state();
    if (state == FLIGHT && privilege == false) {
        ESP_LOGW(TAG, "Abort skipped state == FLIGHT");
        return;
    }

    if (state > FLIGHT) {
        return;
    }

    SM_force_change_state(ABORT);

    ESP_LOGI(TAG, "ABORT");
}

static void mcb_hold_in(uint32_t command, int32_t payload, bool privilege) {
    states_t state = SM_get_current_state();
    if (state == ABORT) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 100);
        return;
    }

    if (state >= FLIGHT) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 100);
        return;
    }

    ESP_LOGI(TAG, "HOLD");
    SM_force_change_state(HOLD);
    gpioexp_camera_turn_off();
}

static void mcb_hold_out(uint32_t command, int32_t payload, bool privilege) {
    states_t state = SM_get_current_state();

    if (state != HOLD) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 100);
        return;
    }

    ESP_LOGI(TAG, "Leaving hold state");

    if (SM_get_previous_state() == COUNTDOWN) {
        SM_force_change_state(RDY_TO_LAUNCH);
    } else {
        SM_change_to_previous_state(true);
    }
    sys_timer_start(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS, TIMER_TYPE_ONE_SHOT);
}

static void mcb_change_lora_frequency_khz(uint32_t command, int32_t payload, bool privilege) {
    if (payload < 4e5 || payload > 1e6) {
        ESP_LOGE(TAG, "Invalid frequency");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    if (lora_change_frequency(payload)  == false) {
        ESP_LOGE(TAG, "Unable to change lora frequency");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    settings_save(SETTINGS_LORA_FREQ_KHZ, payload);
    settings_read_all();

    ESP_LOGI(TAG, "Change frequency");
}

static void mcb_change_lora_transmitting_period(uint32_t command, int32_t payload, bool privilege) {
    ESP_LOGI(TAG, "Transmitting time");
    if (payload <= 500) {
        ESP_LOGE(TAG, "Invalid period");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    if (lora_change_receive_window_period(payload) == false) {
        ESP_LOGE(TAG, "Unable to change period");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    settings_save(SETTINGS_LORA_TRANSMIT_MS, payload);
    settings_read_all();

    ESP_LOGI(TAG, "Transmitting period change to %d ms", payload);
}

static void mcb_change_countdown_time(uint32_t command, int32_t payload, bool privilege) {
    Settings settings = settings_get_all();
    if (payload > settings.ignitTime || payload > -10000) {
        ESP_LOGE(TAG, "Unable to set countdown time");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
    }

    if (settings_save(SETTINGS_COUNTDOWN_TIME, payload) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to set countdown time");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
    }
    settings_read_all();
    settings = settings_get_all();

    liquid_mission_timer_set_disable_val(settings.countdownTime);
}

static void mcb_change_ignition_time(uint32_t command, int32_t payload, bool privilege) {
    Settings settings = settings_get_all();
    if (payload < settings.countdownTime || payload > 0) {
        ESP_LOGE(TAG, "Unable to set ignition time");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
    }

    if (settings_save(SETTINGS_IGNIT_TIME, payload) != ESP_OK) {
        ESP_LOGE(TAG, "Unable to set ignition time");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
    }

    settings_read_all();
}

static void mcb_flash_enable(uint32_t command, int32_t payload, bool privilege) {
    if (payload != 0) {
        settings_save(SETTINGS_FLASH_ON, 1);
    } else {
        settings_save(SETTINGS_FLASH_ON, 0);
    }

    settings_read_all();
}

static void mcb_buzzer_enable(uint32_t command, int32_t payload, bool privilege) {
    if (payload != 0) {
        settings_save(SETTINGS_BUZZER_ON, 1);
        sys_timer_start(TIMER_BUZZER, 2000, TIMER_TYPE_PERIODIC);
    } else {
        settings_save(SETTINGS_BUZZER_ON, 0);
        sys_timer_stop(TIMER_BUZZER);
        buzzer_turn_off();
    }

    settings_read_all();
}

static void mcb_settings_frame(uint32_t command, int32_t payload, bool privilege) {
    lora_send_settings_frame();
}

static void mcb_reset_errors(uint32_t command, int32_t payload, bool privilege) {
    errors_reset_all(1000);
}

static void mcb_foramt_flash(uint32_t command, int32_t payload, bool privilege) {
    if (SM_get_current_state() > RDY_TO_LAUNCH) {
        return;
    }

    FT_format();
    ESP_LOGI(TAG, "FLASH FORMATTED");
}


static void mcb_reset_dev(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    states_t state = SM_get_current_state();
    if (state >= COUNTDOWN && state < ON_GROUND) {
        return;
    }

    recovery_send_cmd(RECOV_RESET, 0);
    esp_restart();
}


static void mcb_reset_disconnect_timer(uint32_t command, int32_t payload, bool privilege) {
    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart timer");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_DISCONNECT_TIMER, 100);
    }
}

static void mcb_cameras_on(uint32_t command, int32_t payload, bool privilege) {
    gpioexp_camera_turn_on();
}

static void mcb_cameras_off(uint32_t command, int32_t payload, bool privilege) {
    gpioexp_camera_turn_off();
}

static cmd_command_t mcb_commands[] = {
    {MCB_STATE_CHANGE,              mcb_state_change},
    {MCB_ABORT,                     mcb_abort},
    {MCB_HOLD_IN,                   mcb_hold_in},
    {MCB_HOLD_OUT,                  mcb_hold_out},
    {MCB_CHANGE_LORA_FREQ,          mcb_change_lora_frequency_khz},
    {MCB_CHANGE_TX_PERIOD,          mcb_change_lora_transmitting_period},
    {MCB_CHANGE_COUNTODWN_TIME,     mcb_change_countdown_time},
    {MCB_CHANGE_IGNITION_TIME,      mcb_change_ignition_time},
    {MCB_FLASH_ENABLE,              mcb_flash_enable},
    {MCB_SETTINGS_FRAME,            mcb_settings_frame},
    {MCB_RESET_ERRORS,              mcb_reset_errors},
    {MCB_FORMAT_FLASH,              mcb_foramt_flash},
    {MCB_BUZZER_ENABLE,             mcb_buzzer_enable},
    {MCB_CAMERAS_ON,                                    mcb_cameras_on},
    {MCB_CAMERAS_OFF,               mcb_cameras_off},
    {MCB_RESET_DEV,                 mcb_reset_dev},
    {MCB_RESET_DISCONNECT_TIMER,    mcb_reset_disconnect_timer},
};

// RECOVERY

static void send_command_recovery(uint32_t command, int32_t payload, bool privilege) {

    if (recovery_send_cmd(command, payload) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }
    else{
        ESP_LOGE(TAG, "Recovery send success :D");
    }
}

static void recov_easymini_arm(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static void recov_easymini_disarm(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static void recov_telemetrum_arm(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static void recov_telemetrum_disarm(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static void recov_force_first_separation(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static void recov_force_second_separation(uint32_t command, int32_t payload, bool privilege) {
    send_command_recovery(command, payload, privilege);
}

static cmd_command_t recovery_commands[] = {
    {RECOV_EASYMINI_ARM,        recov_easymini_arm              },
    {RECOV_EASYMINI_DISARM,     recov_easymini_disarm           },
    {RECOV_TELEMETRUM_ARM,      recov_telemetrum_arm            },
    {RECOV_TELEMETRUM_DISARM,   recov_telemetrum_disarm         },
    {RECOV_FORCE_SECOND_STAGE,  recov_force_first_separation    },
    {RECOV_FORCE_SECOND_STAGE,  recov_force_second_separation   },
};


// MAIN VALVE


static void ov_emval_eth_valve_close(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    // state_id state = SM_get_current_state();
    // if (state > RDY_TO_LAUNCH && state < HOLD) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void ov_emval_eth_valve_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void ov_emval_auto_vent_set(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void ov_emval_auto_vent_off(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void n2val_n2_valve_close(uint32_t command, int32_t payload, bool privilege) {
    state_id state = SM_get_current_state();
    // if (state > RDY_TO_LAUNCH && state < HOLD) {
    //     return;
    // }

    send_command_esp_now(&esp_now_n2_vent_valve, command, payload);
}

static void n2val_n2_valve_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_n2_vent_valve, command, payload);
}

static void oval_n2o_valve_close(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    //state_id state = SM_get_current_state();
    // if (state > RDY_TO_LAUNCH && state < HOLD) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_main_valve, command, payload);
}

static void oval_n2o_valve_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_main_valve, command, payload);
}

static void oval_n2o_dump_valve_arm(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_main_valve, command, payload);
}

static void oval_n2o_dump_valve_disarm(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_main_valve, command, payload);
}

static void ovel_n2o_dump_valve_fire(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_ox_main_valve, command, payload);
}

static void vval_n2_sol_close(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_n2_vent_valve, command, payload);
}

static void vval_n2_sol_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_n2_vent_valve, command, payload);
}

static cmd_command_t n2_vent_valve_commands[] = {
    {N2_SOL_CLOSE,                      vval_n2_sol_close},
    {N2_SOL_OPEN,                       vval_n2_sol_open},
};

static cmd_command_t ox_main_valve_commands[] = {
    {N2O_VALVE_CLOSE,       oval_n2o_valve_close},
    {N2O_VALVE_OPEN,        oval_n2o_valve_open},
    {OX_MAIN_DUMP_VALVE_SOFT_ARM, oval_n2o_dump_valve_arm},
    {OX_MAIN_DUMP_VALVE_SOFT_DISARM, oval_n2o_dump_valve_disarm},
    {OX_MAIN_DUMP_VALVE_FIRE, ovel_n2o_dump_valve_fire},
};

// VENT VALVE

static void ov_emval_n2o_sol_close(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    int32_t tanwa_command = 0x71; // Close N2O valve command for TANWA
    send_command_esp_now(&esp_now_tanwa, tanwa_command, payload);
    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void ov_emval_n2o_sol_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }
    int32_t command_tanwa = 0x70; // Open N2O valve command for TANWA
    send_command_esp_now(&esp_now_tanwa, command_tanwa, payload);
    
    send_command_esp_now(&esp_now_ox_vent_eth_main_valves, command, payload);
}

static void vval_eth_sol_close(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_eth_vent_valve, command, payload);
}

static void vval_eth_sol_open(uint32_t command, int32_t payload, bool privilege) {
    // if (privilege == false) {
    //     return;
    // }

    send_command_esp_now(&esp_now_eth_vent_valve, command, payload);
}

static cmd_command_t eth_vent_valve_commands[] = {
    {ETH_SOL_CLOSE,                    vval_eth_sol_close},
    {ETH_SOL_OPEN,                      vval_eth_sol_open},
};

static cmd_command_t ox_vent_eth_main_valves_commands[] = {
    {N2O_SOL_CLOSE,                ov_emval_n2o_sol_close},
    {N2O_SOL_OPEN,                  ov_emval_n2o_sol_open},
    {ETH_VALVE_CLOSE,            ov_emval_eth_valve_close},
    {ETH_VALVE_OPEN,              ov_emval_eth_valve_open},
    {OX_VENT_AUTO_VENT_OFF,      ov_emval_auto_vent_off},
    {OX_VENT_AUTO_VENT_SET,       ov_emval_auto_vent_set},
};

static cmd_command_t tanwa_commands[] = {
    {TANWA_STATE_CHANGE,            tanwa_process_command},
    {TANWA_ABORT,                   tanwa_process_command},
    {TANWA_HOLD_IN,                 tanwa_process_command},
    {TANWA_HOLD_OUT,                tanwa_process_command},
    {TANWA_LORA_TRANSMIT_F,         tanwa_process_command},
    {TANWA_LORA_TRANSMIT_T,         tanwa_process_command},
    {TANWA_SEND_SETTINGS,           tanwa_process_command},
    {TANWA_RESET,                   tanwa_process_command},
    {TANWA_SOFT_ARM,                tanwa_process_command},
    {TANWA_SOFT_DISARM,             tanwa_process_command},
    {TANWA_RESTART_WEIGHT,          tanwa_process_command},
    {TANWA_CALIBRATE_WEIGHT,        tanwa_process_command},
    {TANWA_TARE_WEIGHT,             tanwa_process_command},
    {TANWA_SET_CAL_FACTOR_WEIGHT,   tanwa_process_command},
    {TANWA_SET_OFFSET_WEIGHT,       tanwa_process_command},
    {TANWA_N2O_FILL_OPEN,           tanwa_process_command},
    {TANWA_N2O_FILL_CLOSE,          tanwa_process_command},
    {TANWA_N2O_FILL_OPEN_TIME,      tanwa_process_command},
    {TANWA_N2O_DEPR_OPEN,           tanwa_process_command},
    {TANWA_N2O_DEPR_CLOSE,          tanwa_process_command},
    {TANWA_N2O_DEPR_OPEN_TIME,      tanwa_process_command},
    {TANWA_QD_N2O_UNPLUG,           tanwa_process_command},
    {TANWA_QD_N2O_STOP,             tanwa_process_command},
    {TANWA_QD_N2_UNPLUG,            tanwa_process_command},
    {TANWA_QD_N2_STOP,              tanwa_process_command},
    {TANWA_HEATING_TANK_START,      tanwa_process_command},
    {TANWA_HEATING_TANK_STOP,       tanwa_process_command},
    {TANWA_HEATING_VALVE_START,     tanwa_process_command},
    {TANWA_HEATING_VALVE_STOP,      tanwa_process_command},
    {TANWA_N2_FILL_OPEN,            tanwa_process_command},
    {TANWA_N2_FILL_CLOSE,           tanwa_process_command},
    {TANWA_N2_FILL_OPEN_TIME,       tanwa_process_command},
    {TANWA_N2_DEPR_OPEN,            tanwa_process_command},
    {TANWA_N2_DEPR_CLOSE,           tanwa_process_command},
    {TANWA_N2_DEPR_OPEN_TIME,       tanwa_process_command},
    {TANWA_FIRE,                    tanwa_process_command},
};

// DEVICES
#define SIZE_OF(x) sizeof(x) / sizeof(x[0])

static cmd_device_t devices[] = {
    {DEVICE_MCB,            mcb_commands,            SIZE_OF(mcb_commands)},
    {DEVICE_RECOVERY,       recovery_commands,       SIZE_OF(recovery_commands)},
    {DEVICE_N2_VENT_VALVE,    n2_vent_valve_commands,    SIZE_OF(n2_vent_valve_commands)},
    {DEVICE_ETH_VENT_VALVE,    eth_vent_valve_commands,    SIZE_OF(eth_vent_valve_commands)},
    {DEVICE_OX_MAIN_VALVE,  ox_main_valve_commands,  SIZE_OF(ox_main_valve_commands)},
    {DEVICE_OX_VENT_ETH_MAIN_VALVES, ox_vent_eth_main_valves_commands, SIZE_OF(ox_vent_eth_main_valves_commands)},
    {DEVICE_TANWA,          tanwa_commands,          SIZE_OF(tanwa_commands)},
};

static cmd_t commands = {
    .lora_dev_id = LORA_DEV_ID,
    .devices = devices,
    .number_of_devices = SIZE_OF(devices),
};

bool lora_cmd_init(void) {
    return cmd_init_lora_mode(&commands);
}

bool lora_cmd_process_command(cmd_lora_dev_id lora_dev_id, cmd_sys_dev_id dev_id, cmd_message_t *message) {
    return cmd_process_lora_command(&commands, lora_dev_id, dev_id, message);
}


