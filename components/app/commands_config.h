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


#endif