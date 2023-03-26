// Copyright 2022 PWrInSpace, Kuba
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_CMD_BROADCAST_DEV_ID 0x00
#define RF_CMD_PRIVILAGE_MASK 0x01


typedef uint32_t rf_cmd_lora_dev_id;
typedef uint32_t rf_cmd_sys_dev_id;
typedef uint32_t rf_cmd_command_id;
typedef void (*rf_cmd_on_command_receive)(uint32_t command, int32_t payload, bool privilage);

typedef struct {
    rf_cmd_command_id command_id;
    rf_cmd_on_command_receive on_command_receive_fnc;
} rf_cmd_command_t;

typedef struct {
    rf_cmd_sys_dev_id dev_id;
    rf_cmd_command_t *commands;
    size_t number_of_commands;
} rf_cmd_device_t;

typedef union {
    struct command {
        uint32_t command;
        int32_t payload;
    } cmd;

    uint8_t raw[sizeof(struct command)];
} rf_cmd_message_t;


typedef struct {
    rf_cmd_lora_dev_id lora_dev_id;  // lora dev id
    rf_cmd_device_t *devices;  // pointer to static allocated array of commands
    size_t number_of_devices;   // numbner of commands
} rf_cmd_lora_config_t;


typedef struct {
    rf_cmd_device_t *devices;  // pointer to static allocated array of commands
    size_t number_of_devices;   // numbner of commands
} rf_cmd_config_t;



/**
 * @brief Initialize commands api without task

 *
 * @param cfg pointer to config
 * @return true :)
 * @return false :C
 */
bool rf_cmd_init_standard_mode(rf_cmd_config_t *cfg);

/**
 * @brief Initialize commands api in lora mode
 * 
 * @param cfg pointer to config
 * @return true :D
 * @return false :C
 */
bool rf_cmd_init_lora_mode(rf_cmd_lora_config_t *cfg);

/**
 * @brief Process command in standard mode
 *
 * @param dev_id system dev id
 * @param message message to process
 * @return true :D
 * @return false :C
 */
bool rf_cmd_process_command(rf_cmd_sys_dev_id dev_id, rf_cmd_message_t *message);

/**
 * @brief Process command in lora mode
 * 
 * @param lora_dev_id lora dev id
 * @param dev_id system device id
 * @param message message to process
 * @return true :D
 * @return false :C
 */
bool rf_cmd_process_lora_command(
    rf_cmd_lora_dev_id lora_dev_id,
    rf_cmd_sys_dev_id dev_id,
    rf_cmd_message_t *message);

/**
 * @brief Creater message
 *
 * @param rf_cmd rf_cmd
 * @param payload payload
 * @return rf_cmd_message_t created message
 */
rf_cmd_message_t rf_cmd_create_message(uint32_t command, int32_t payload);



#ifdef __cplusplus
}
#endif

#endif
