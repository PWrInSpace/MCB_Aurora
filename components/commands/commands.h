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

typedef enum {
    COMMAND_NOT_FOUND = 1,
} COMMAND_ERROR;

typedef uint8_t rf_cmd_device_id_t;
typedef uint32_t rf_cmd_command_id_t;
typedef void (*rf_cmd_on_command_receive)(uint32_t command, int32_t payload, bool sudo);
typedef void (*rf_cmd_on_task_error)(COMMAND_ERROR error);


typedef struct {
    rf_cmd_command_id_t command_id;
    rf_cmd_on_command_receive on_command_receive_fnc;
} rf_cmd_command_t;

typedef struct {
    rf_cmd_device_id_t dev_id;
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
    rf_cmd_device_t *devices;  // pointer to static allocated array of commands
    size_t number_of_devices;   // numbner of commands
    // function calls on error, can be NULL if you dont want to use
    rf_cmd_on_task_error task_error_fnc;

    // uint32_t stack_depth;   // task stack_depth
    // BaseType_t core_id;     // task core_id
    // UBaseType_t priority;   // task priority
    // uint8_t queue_size;     // queueu size
} rf_cmd_config_t;

/**
 * @brief Initialize commands api without task
 *
 * @param cfg pointer to config
 * @return true :)
 * @return false :C
 */
bool rf_cmd_init(rf_cmd_config_t *cfg);

/**
 * @brief Initialize rf_cmd task
 *
 * @param cfg pointer to config
 * @return true :)
 * @return false :C
 */
bool rf_cmd_init_with_task(rf_cmd_config_t *cfg);


/**
 * @brief Send rf_cmd to task for processing
 *
 * @param rf_cmd pointer to rf_cmd
 * @return true :D
 * @return false :C
 */
bool rf_cmd_send_rf_cmd_for_processing(rf_cmd_message_t *command);

/**
 * @brief Process rf_cmd without task
 * 
 * @param rf_cmd pointer to rf_cmd
 * @return true :D
 * @return false :C
 */
bool rf_cmd_process_rf_cmd(rf_cmd_message_t *command);

/**
 * @brief Creater message
 *
 * @param rf_cmd rf_cmd
 * @param payload payload
 * @return rf_cmd_message_t created message
 */
rf_cmd_message_t rf_cmd_create_message(uint32_t command, int32_t payload);

/**
 * @brief Terminate task
 *
 */
void rf_cmd_terminate_task(void);

#ifdef __cplusplus
}
#endif

#endif
