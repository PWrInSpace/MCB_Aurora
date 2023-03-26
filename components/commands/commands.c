// Copyright 2022 PWrInSpace, Kuba


#include "commands.h"
#include <string.h>

#include "esp_log.h"

#define TAG "CMD"

static struct {
    rf_cmd_lora_dev_id lora_dev_id;
    rf_cmd_device_t *devices;
    size_t number_of_devices;
} gb = {
    .devices = NULL,
    .number_of_devices = 0,
};

static bool init(rf_cmd_device_t *devices, size_t dev_nb, rf_cmd_lora_dev_id lora_dev_id) {
    assert(devices != NULL);
    assert(dev_nb > 0);
    if (devices == NULL || dev_nb == 0) {
        return false;
    }

    gb.devices = devices;
    gb.number_of_devices = dev_nb;
    gb.lora_dev_id = lora_dev_id;

    return true;
}

bool rf_cmd_init_standard_mode(rf_cmd_config_t *cfg) {
    return init(cfg->devices, cfg->number_of_devices, RF_CMD_BROADCAST_DEV_ID);
}

bool rf_cmd_init_lora_mode(rf_cmd_lora_config_t *cfg) {
    return init(cfg->devices, cfg->number_of_devices, cfg->lora_dev_id);
}

static bool get_device_index(rf_cmd_sys_dev_id dev_id, size_t *dev_index) {
    for (size_t i = 0; i < gb.number_of_devices; ++i) {
        if (gb.devices[i].dev_id == dev_id) {
            *dev_index = i;
            return true;
        }
    }
    return false;
}

static bool find_command_and_execute(size_t dev_index, rf_cmd_message_t *received_command,
                                     bool privilage) {
    rf_cmd_device_t* dev = &gb.devices[dev_index];
    for (size_t i = 0; i < dev->number_of_commands; ++i) {
        if (dev->commands[i].command_id == received_command->cmd.command) {
            if (dev->commands[i].on_command_receive_fnc != NULL) {
                dev->commands[i].on_command_receive_fnc(
                    received_command->cmd.command, received_command->cmd.payload, privilage);
            }
            return true;
        }
    }

    return false;
}

static bool process_command(rf_cmd_sys_dev_id dev_id, rf_cmd_message_t *message, bool privilage) {
    size_t index = 0;
    if (get_device_index(dev_id, &index) == false) {
        return false;
    }
    ESP_LOGI(TAG, "INDEX %d", index);
    return find_command_and_execute(index, message, privilage);
}

bool rf_cmd_process_command(rf_cmd_sys_dev_id dev_id, rf_cmd_message_t *message) {
    return process_command(dev_id, message, false);
}

static bool check_lora_dev_id(rf_cmd_lora_dev_id dev_id) {
    if (dev_id == 0x00) {
        return true;
    }
    // remove privilage mask and check id
    return (dev_id >> 1) == (gb.lora_dev_id >> 1) ? true : false;
}

static bool check_dev_id_privilage_mode(rf_cmd_lora_dev_id dev_id) {
    return (dev_id & RF_CMD_PRIVILAGE_MASK) > 0 ? true : false;
}

bool rf_cmd_process_lora_command(rf_cmd_lora_dev_id lora_dev_id, rf_cmd_sys_dev_id dev_id,
                                 rf_cmd_message_t *message) {
    if (gb.lora_dev_id == RF_CMD_BROADCAST_DEV_ID) {
        ESP_LOGE(TAG, "Library hasn't been initialized in LoRa mode");
        return false;
    }

    if (check_lora_dev_id(lora_dev_id) == false) {
        ESP_LOGE(TAG, "Incorrect lora dev id");
        return false;
    }

    if (check_dev_id_privilage_mode(lora_dev_id) == true) {
        return process_command(dev_id, message, true);
    }

    return process_command(dev_id, message, false);
}

rf_cmd_message_t rf_cmd_create_message(uint32_t command, int32_t payload) {
    rf_cmd_message_t message = {.cmd.command = command, .cmd.payload = payload};

    return message;
}