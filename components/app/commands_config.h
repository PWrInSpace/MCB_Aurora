// Copyright 2022 PWrInSpace, Kuba
#ifndef COMMANDS_CONFIG_H
#define COMMANDS_CONFIG_H

#include "commands.h"

/**
 * @brief Initialzie lora commands
 * 
 * @return true :D
 * @return false :C
 */
bool lora_cmd_init(void);

/**
 * @brief Process received command from lora
 *
 * @param lora_dev_id lora_dev_id
 * @param dev_id receiver_device_id
 * @param message message
 * @return true :D
 * @return false :C
 */
bool lora_cmd_process_command(cmd_lora_dev_id lora_dev_id,
                            cmd_sys_dev_id dev_id,
                            cmd_message_t *message);

typedef enum {
    MCB_STATE_CHANGE = 0x00,
    MCB_ABORT = 0x01,
    MCB_HOLD = 0x02,
    MCB_CHANGE_LORA_FREQ = 0x10,
    MCB_CHANGE_TX_PERIOD = 0x11,
    MCB_CHANGE_COUNTODWN_TIME = 0x12,
    MCB_CHANGE_IGNITION_TIME = 0x13,
    MCB_FLASH_ENABLE = 0x14,
    MCB_RESET_DISCONNECT_TIMER = 0xFF,
} mcb_commands_t;

typedef enum {
    RECOV_EASYMINI_ARM = 0x01,
    RECOV_EASYMINI_DISARM = 0x02,
    RECOV_TELEMETRUM_ARM = 0x03,
    RECOV_TELEMETRUM_DISARM = 0x04,
    RECOV_FORCE_FIRST_STAGE = 0xa5,
    RECOV_FORCE_SECOND_STAGE = 0x5a,
} recovery_commands_t;


typedef enum {
    MAIN_VALVE_CLOSE = 0x00,
    MAIN_VALVE_OPEN = 0x01,
    MAIN_VALVE_OPEN_ANGLE = 0x03,
    MAIN_VALVE_CALIBRATE = 0x04
} main_valve_commands_t;

typedef enum {
    VENT_VALVE_CLOSE = 0x00,
    VENT_VALVE_OPEN = 0x01,
    VENT_VALVE_OPEN_ANGLE = 0x03,
    VENT_VALVE_AUTOPRESS_TIME = 0x13,
    VENT_VALVE_AUTOPRESS_LIMIT = 0x14,
} vent_valve_commands_t;

#endif