// Copyright 2022 PWrInSpace, Kuba

#include "commands_config.h"
#include "esp_log.h"
#include "state_machine_config.h"
#include "lora_task.h"
#include "esp_now_config.h"
#include "errors_config.h"
#include "rocket_data.h"
#include "recovery_task_config.h"
#include "system_timer_config.h"

#define TAG "CMD"
// COMMANDS
// https://docs.google.com/spreadsheets/d/1kvS3BirYGmhAizmF42UDyF-SwWfSJDnYGSNufSPmQ5g/edit#gid=1424247026

// MCB
static bool state_change_check_countdown(void) {
    recovery_data_t data = rocket_data_get_recovery();
    if (data.isArmed == false || data.isTeleActive == false) {
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_NOT_ARMED, 100);
        return false;
    }

    if (rocket_data_woken_up() == false) {
        ESP_LOGE(TAG, "On or more devices are sleeping");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_WAKE_UP, 100);
        return false;
    }

    return true;
}


static void mcb_state_change(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Changing state to -> %d", payload);

    if (payload == COUNTDOWN && privilage == false) {
        if (state_change_check_countdown() == false) {
            return;
        }
    }

    if (SM_change_state(payload) != SM_OK) {
        ESP_LOGE(TAG, "Unable to change state");
        return;
    }

    ESP_LOGI(TAG, "STATE CHANGED");
}

static void mcb_abort(uint32_t command, int32_t payload, bool privilage) {
    states_t state = SM_get_current_state();
    if (state == FLIGHT && privilage == false) {
        ESP_LOGW(TAG, "Abort skipped state == FLIGHT");
        return;
    }

    if (state > FLIGHT) {
        return;
    }

    SM_force_change_state(ABORT);

    ESP_LOGI(TAG, "ABORT");
}

static void mcb_hold(uint32_t command, int32_t payload, bool privilage) {
    states_t state = SM_get_current_state();
    if (state == ABORT) {
        return;
    }


    if (state == HOLD) {
        ESP_LOGI(TAG, "Leaving hold state");
        if (SM_get_previous_state() == COUNTDOWN) {
            SM_force_change_state(RDY_TO_LAUNCH);
        } else {
            SM_change_to_previous_state(true);
        }
    } else if (state < FLIGHT) {
        SM_force_change_state(HOLD);
        ESP_LOGI(TAG, "HOLD");
    }
}

static void mcb_change_lora_frequency_khz(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Change frequency");
}

static void mcb_change_lora_transmiting_period(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Transmiting time");
    if (payload <= 0) {
        ESP_LOGE(TAG, "Invalid period");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    if (lora_change_receive_window_period(payload) == false) {
        ESP_LOGE(TAG, "Unable to change period");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_OPTION_VALUE, 100);
        return;
    }

    ESP_LOGI(TAG, "Tranismiting period change to %d ms", payload);
}

static void mcb_change_countdown_time(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Countdown time");
}

static void mcb_change_ignition_time(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Change ignition time");
}

static void mcb_flash_enable(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Flash enable");
}

static void mcb_reset_disconnect_timer(uint32_t command, int32_t payload, bool privilage) {
    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart timer");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_DISCONNECT_TIMER, 100);
    }
}

static cmd_command_t mcb_commands[] = {
    {MCB_STATE_CHANGE,              mcb_state_change},
    {MCB_ABORT,                     mcb_abort},
    {MCB_HOLD,                      mcb_hold},
    {MCB_CHANGE_LORA_FREQ,          mcb_change_lora_frequency_khz},
    {MCB_CHANGE_TX_PERIOD,          mcb_change_lora_transmiting_period},
    {MCB_CHANGE_COUNTODWN_TIME,     mcb_change_countdown_time},
    {MCB_CHANGE_IGNITION_TIME,      mcb_change_ignition_time},
    {MCB_FLASH_ENABLE,              mcb_flash_enable},
    {MCB_RESET_DISCONNECT_TIMER,    mcb_reset_disconnect_timer},
};

// RECOVERY
static void send_command_recovery(uint32_t command, int32_t payload) {
    if (recovery_send_cmd(command, payload) == false) {
        errors_add(ERROR_TYPE_RECOVERY, ERROR_RECOV_TRANSMIT, 100);
        ESP_LOGE(TAG, "Recovery send error :C");
    }
}


static void recov_easymini_arm(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
}

static void recov_easymini_disarm(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
}

static void recov_telemetrum_arm(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
}

static void recov_telemetrum_disarm(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
}

static void recov_force_first_separation(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
}

static void recov_force_second_separation(uint32_t command, int32_t payload, bool privilage) {
    send_command_recovery(command, payload);
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
static void send_command_esp_now(const ENA_device_t *dev, uint32_t command, int32_t payload) {
    cmd_message_t mess = cmd_create_message(command, payload);
    if (ENA_send(dev, mess.raw, sizeof(mess), 3) != ESP_OK) {
        ESP_LOGI(TAG, "Unable to send command %d, %d", command, payload);
        return;
    }
}

static void mval_valve_close(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_main_valve, command, payload);
}

static void mval_valve_open(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_main_valve, command, payload);
}


static void mval_valve_open_angle(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_main_valve, command, payload);
}

static void mval_valve_calibrate(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_main_valve, command, payload);
}

static cmd_command_t main_valve_commands[] = {
    {MAIN_VALVE_CLOSE,          mval_valve_close        },
    {MAIN_VALVE_OPEN,           mval_valve_open         },
    {MAIN_VALVE_OPEN_ANGLE,     mval_valve_open_angle   },
    {MAIN_VALVE_CALIBRATE,      mval_valve_calibrate    },
};


// VENT VALVE

static void vval_valve_close(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_vent_valve, command, payload);
}

static void vval_valve_open(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_vent_valve, command, payload);
}


static void vval_valve_open_angle(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_vent_valve, command, payload);
}

static void vval_autopress_time(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_vent_valve, command, payload);
}


static void vval_autopress_limit(uint32_t command, int32_t payload, bool privilage) {
    send_command_esp_now(&esp_now_vent_valve, command, payload);
}

static cmd_command_t vent_valve_commands[] = {
    {VENT_VALVE_CLOSE,              vval_valve_close        },
    {VENT_VALVE_OPEN,               vval_valve_open         },
    {VENT_VALVE_OPEN,               vval_valve_open_angle   },
    {VENT_VALVE_AUTOPRESS_TIME,     vval_autopress_time     },
    {VENT_VALVE_AUTOPRESS_LIMIT,    vval_autopress_limit    },
};

// DEVICES

typedef enum {
    MCB = 0x01,
    RECOVERY = 0x02,
    TANWA = 0x03,
    MAIN_VALVE = 0x04,
    VENT_VALVE = 0x05,
    PITOT = 0x06,
} device_t;

#define SIZE_OF(x) sizeof(x) / sizeof(x[0])

static cmd_device_t devices[] = {
    {MCB,           mcb_commands,           SIZE_OF(mcb_commands)},
    {RECOVERY,      recovery_commands,      SIZE_OF(recovery_commands)},
    {MAIN_VALVE,    main_valve_commands,    SIZE_OF(main_valve_commands)},
    {VENT_VALVE,    vent_valve_commands,    SIZE_OF(vent_valve_commands)},
};

static cmd_t commands = {
    .lora_dev_id = 0x02,
    .devices = devices,
    .number_of_devices = SIZE_OF(devices),
};

bool lora_cmd_init(void) {
    return cmd_init_lora_mode(&commands);
}

bool lora_cmd_process_command(cmd_lora_dev_id lora_dev_id,
                            cmd_sys_dev_id dev_id,
                            cmd_message_t *message) {
    return cmd_process_lora_command(&commands, lora_dev_id, dev_id, message);
}


