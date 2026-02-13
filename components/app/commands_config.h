// Copyright 2022 PWrInSpace, Kuba
#ifndef COMMANDS_CONFIG_H
#define COMMANDS_CONFIG_H

#include "commands.h"

#define LORA_DEV_ID 0x02

/**
 * @brief Initialzie lora commands
 * 
 * @return true :D
 * @return false :C
 */
// bool lora_TANWA_init(void);

/**
 * @brief Process received command from lora
 *
 * @param lora_dev_id lora_dev_id
 * @param dev_id receiver_device_id
 * @param message message
 * @return true :D
 * @return false :C
 */
// bool lora_TANWA_process_command(TANWA_lora_dev_id lora_dev_id,
//                             TANWA_sys_dev_id dev_id,
//                             TANWA_message_t *message);


bool lora_cmd_process_command(cmd_lora_dev_id lora_dev_id,
                            cmd_sys_dev_id dev_id,
                            cmd_message_t *message);


typedef enum {
    DEVICE_MCB = 0x01,
    DEVICE_RECOVERY = 0x02,
    DEVICE_TANWA = 0x03,
    DEVICE_N2_MAIN_VALVE = 0x04,
    DEVICE_VENT_VALVES = 0x05,
    DEVICE_OX_MAIN_VALVE = 0x06,
    DEVICE_OX_VENT_ETH_MAIN_VALVES = 0x07,
    PITOT = 0x08,
} device_t;

typedef enum {
    MCB_STATE_CHANGE = 0x00,
    MCB_ABORT = 0x01,
    MCB_HOLD_IN = 0x02,
    MCB_HOLD_OUT = 0x03,
    MCB_CHANGE_LORA_FREQ = 0x10,
    MCB_CHANGE_TX_PERIOD = 0x11,
    MCB_CHANGE_COUNTODWN_TIME = 0x12,
    MCB_CHANGE_IGNITION_TIME = 0x13,
    MCB_FLASH_ENABLE = 0x14,
    MCB_SETTINGS_FRAME = 0x15,
    MCB_RESET_ERRORS = 0x16,
    MCB_FORMAT_FLASH = 0x17,
    MCB_BUZZER_ENABLE = 0x18,
    MCB_RESET_DEV = 0x82,
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
    TANWA_STATE_CHANGE = 0x00,
    TANWA_ABORT = 0x01,
    TANWA_HOLD_IN = 0x02,
    TANWA_HOLD_OUT = 0x03,
    TANWA_LORA_TRANSMIT_F = 0x10,
    TANWA_LORA_TRANSMIT_T = 0x11,
    TANWA_SEND_SETTINGS = 0x15,
    TANWA_RESET = 0x80,
    TANWA_SOFT_ARM = 0x29,
    TANWA_SOFT_DISARM = 0x30,
    TANWA_RESTART_WEIGHT = 0x31,
    TANWA_CALIBRATE_WEIGHT = 0x34,
    TANWA_TARE_WEIGHT = 0x35,
    TANWA_SET_CAL_FACTOR_WEIGHT = 0x36,
    TANWA_SET_OFFSET_WEIGHT = 0x37,
    TANWA_N2O_FILL_OPEN = 0x42,
    TANWA_N2O_FILL_CLOSE = 0x43,
    TANWA_N2O_FILL_OPEN_TIME = 0x44,
    TANWA_N2O_DEPR_OPEN = 0x45,
    TANWA_N2O_DEPR_CLOSE = 0x46,
    TANWA_N2O_DEPR_OPEN_TIME = 0x47,
    TANWA_QD_N2O_UNPLUG = 0x48,
    TANWA_QD_N2O_STOP = 0x49,
    TANWA_QD_N2_UNPLUG = 0x4A,
    TANWA_QD_N2_STOP = 0x4B,
    TANWA_HEATING_TANK_START = 0x4C,
    TANWA_HEATING_TANK_STOP = 0x4D,
    TANWA_HEATING_VALVE_START = 0x4E,
    TANWA_HEATING_VALVE_STOP = 0x4F,
    TANWA_N2_FILL_OPEN = 0x50,
    TANWA_N2_FILL_CLOSE = 0x51,
    TANWA_N2_FILL_OPEN_TIME = 0x52,
    TANWA_N2_DEPR_OPEN = 0x53,
    TANWA_N2_DEPR_CLOSE = 0x54,
    TANWA_N2_DEPR_OPEN_TIME = 0x55,
    TANWA_FIRE = 0x60, //FIREBALL, DOO DOO DOO DOO DO DOO (FIREBALL) ~PITBULL 2k10
} tanwa_commands_t;

typedef enum {
    N2O_VALVE_CLOSE = 0x00,
    N2O_VALVE_OPEN = 0x01,
    ETH_VALVE_CLOSE = 0x02,
    ETH_VALVE_OPEN = 0x03,
    N2_VALVE_CLOSE = 0x04,
    N2_VALVE_OPEN = 0x05,

    N2O_SOL_CLOSE = 0x10,
    N2O_SOL_OPEN = 0x11,
    ETH_SOL_CLOSE = 0x12,
    ETH_SOL_OPEN = 0x13,
    N2_SOL_CLOSE = 0x14,
    N2_SOL_OPEN = 0x15,

    VALVE_DZIDA = 0x69,

} valve_commands_t;

#endif