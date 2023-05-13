#include "recovery.h"
#include "i2c.h"

bool recovery_send_data(uint8_t* message, size_t len) {
    return i2c_com_write(RECOVERY_ADDRESS, 0x00, message, len);
}

bool recovery_send_cmd(uint8_t command, size_t payload) {
    uint8_t tmp[2];
    tmp[0] = command;
    tmp[1] = payload;
    return i2c_com_write(RECOVERY_ADDRESS, 0x00, tmp, sizeof(tmp));
}

bool recovery_read_data(uint8_t* message, size_t len) {
    return i2c_com_write(RECOVERY_ADDRESS, 0x00, message, len);
}