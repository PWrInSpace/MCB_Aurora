#ifndef RECOVERY_H
#define RECOVERY_H

#define RECOVERY_ADDRESS 0x04

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool recovery_send_data(uint8_t* message, size_t len);
bool recovery_send_cmd(uint8_t command, size_t payload);
bool recovery_read_data(uint8_t* message, size_t len);

#endif