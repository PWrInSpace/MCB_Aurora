#ifndef RECOVERY_TASK_CONFIG_H
#define RECOVERY_TASK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define RECOVERY_ADDRESS 0x30  // move to kconfig

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

#endif