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

typedef void (*cmd_on_command_receive)(uint32_t command, int32_t payload);
typedef void (*cmd_on_task_error)(COMMAND_ERROR error);

typedef struct {
    uint32_t command;
    cmd_on_command_receive receive_fnc;
} command_handle_t;

typedef union {
    struct command {
        uint32_t command;
        int32_t payload;
    } cmd;

    uint8_t raw[sizeof(struct command)];
} command_message_t;

typedef struct {
    command_handle_t *commands;  // pointer to static allocated array of commands
    size_t number_of_commands;   // numbner of commands
    // function calls on error, can be NULL if you dont want to use
    cmd_on_task_error task_error_fnc;

    uint32_t stack_depth;   // task stack_depth
    BaseType_t core_id;     // task core_id
    UBaseType_t priority;   // task priority
    uint8_t queue_size;     // queueu size
} command_config_t;

/**
 * @brief Initialize commands api without task
 *
 * @param cfg pointer to config
 * @return true :)
 * @return false :C
 */
bool CMD_init(command_config_t *cfg);

/**
 * @brief Initialize command task
 *
 * @param cfg pointer to config
 * @return true :)
 * @return false :C
 */
bool CMD_init_with_task(command_config_t *cfg);


/**
 * @brief Send command to task for processing
 *
 * @param command pointer to command
 * @return true :D
 * @return false :C
 */
bool CMD_send_command_for_processing(command_message_t *command);

/**
 * @brief Process command without task
 * 
 * @param command pointer to command
 * @return true :D
 * @return false :C
 */
bool CMD_process_command(command_message_t *command);

/**
 * @brief Creater message
 *
 * @param command command
 * @param payload payload
 * @return command_message_t created message
 */
command_message_t CMD_create_message(uint32_t command, int32_t payload);

/**
 * @brief Terminate task
 *
 */
void CMD_terminate_task(void);

#ifdef __cplusplus
}
#endif

#endif
