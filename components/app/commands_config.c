// Copyright 2022 PWrInSpace, Kuba

#include "commands_config.h"
#include "esp_log.h"
#include "state_machine_config.h"
#include "lora_task.h"
#include "esp_now_config.h"

#define TAG "CMD"
// COMMANDS
// https://docs.google.com/spreadsheets/d/1kvS3BirYGmhAizmF42UDyF-SwWfSJDnYGSNufSPmQ5g/edit#gid=1424247026

// MCB
static void mcb_state_change(uint32_t command, int32_t payload, bool privilage) {
    ESP_LOGI(TAG, "Changing state to -> %d", payload);
    if (SM_change_state(payload) != SM_OK) {
        ESP_LOGE(TAG, "Unable to change state");
        return;
    }
    ESP_LOGI(TAG, "STATE CHANGE");
}

static void mcb_abort(uint32_t command, int32_t payload, bool privilage) {
    SM_force_change_state(ABORT);
    ESP_LOGI(TAG, "ABORT");
}

static void mcb_hold(uint32_t command, int32_t payload, bool privilage) {
    if (SM_get_current_state() == HOLD) {
        ESP_LOGI(TAG, "Leaving hold state");
        if (SM_get_previous_state() == COUNTDOWN) {
            SM_force_change_state(RDY_TO_LAUNCH);
        } else {
            SM_change_to_previous_state(false);
        }
    } else {
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
        // error
        return;
    }

    if (lora_change_receive_window_period(payload) == false) {
        ESP_LOGE(TAG, "Unable to change period");
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

static cmd_command_t mcb_commands[] = {
    {0x00, mcb_state_change},
    {0x01, mcb_abort},
    {0x02, mcb_hold},
    {0x10, mcb_change_lora_frequency_khz},
    {0x11, mcb_change_lora_transmiting_period},
    {0x12, mcb_change_countdown_time},
    {0x13, mcb_change_ignition_time},
    {0x14, mcb_flash_enable},
    {0xFF, NULL},
};

// RECOVERY

static void recov_easymini_arm(uint32_t command, int32_t payload, bool privilage) {
}

static void recov_easymini_disarm(uint32_t command, int32_t payload, bool privilage) {

}

static void recov_telemetrum_arm(uint32_t command, int32_t payload, bool privilage) {

}

static void recov_telemetrum_disarm(uint32_t command, int32_t payload, bool privilage) {

}

static void recov_force_first_separation(uint32_t command, int32_t payload, bool privilage) {

}

static void recov_force_second_separation(uint32_t command, int32_t payload, bool privilage) {

}

static cmd_command_t recovery_commands[] = {
    {0x01, recov_easymini_arm},
    {0x02, recov_easymini_disarm},
    {0x03, recov_telemetrum_arm},
    {0x04, recov_telemetrum_disarm},
    {0xa5, recov_force_first_separation},
    {0x5a, recov_force_second_separation},
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
    {0x00, mval_valve_close},
    {0x01, mval_valve_open},
    {0x03, mval_valve_open_angle},
    {0x04, mval_valve_calibrate},
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
    {0x00, vval_valve_close},
    {0x01, vval_valve_open},
    {0x03, vval_valve_open_angle},
    {0x13, vval_autopress_time},
    {0x14, vval_autopress_limit},
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


