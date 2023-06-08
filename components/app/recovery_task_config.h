#ifndef RECOVERY_TASK_CONFIG_H
#define RECOVERY_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "rocket_data.h"

#define RECOVERY_ADDRESS 0x30  // move to kconfig

typedef void (*recovery_task_process_fnc)(recovery_data_t *data);

/**
 * @brief Initalize recovery task
 *
 * @return true :D
 * @return false :C
 */
bool initialize_recovery(void);

/**
 * @brief Send command to recovery board
 *
 * @param command command
 * @param payload payload
 * @return true :D
 * @return false NACK
 */
bool recovery_send_cmd(uint32_t command, int32_t payload);

/**
 * @brief Change recovery task process fnc
 *
 * @param fnc process to function
 * @return true :D
 * @return false :C
 */
bool recovery_change_process_fnc(recovery_task_process_fnc fnc);

/**
 * @brief Delete recovery task process fnc
 *
 * @return true :D
 * @return false :C
 */
bool recovery_remove_process_fnc(void);

#endif