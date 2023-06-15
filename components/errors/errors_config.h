// Copyright 2022 PWrInSpace, Kuba
#ifndef ERRORS_CONFIG_H
#define ERRORS_CONFIG_H

#include "errors.h"

typedef enum {
    ERROR_TYPE_ESP_NOW = 0,
    ERROR_TYPE_MEMORY,
    ERROR_TYPE_MCB,
    ERROR_TYPE_SENSORS,
    ERROR_TYPE_RECOVERY,
    ERROR_TYPE_LAST_EXCEPTION,
} errors_types_t;

// DONE
typedef enum {
    ERROR_ESP_NOW_OK = 0,
    ERROR_ESP_NOW_SEND = (1 << 0),
    ERROR_ESP_NOW_RECEIVE = (1 << 2),
    ERROR_ESP_NOW_DEVICE = (1 << 3),
    ERROR_ESP_UNKNOWN = (1 << 4)
} errors_esp_now_t;

// DONE
typedef enum {
    ERROR_MEMORY_OK = 0,
    ERROR_SD_WRITE = (1 << 0),
    ERROR_FLASH_WRITE = (1 << 1),
    ERROR_FLASH_FILE = (1 << 3),
    ERROR_FLASH_FORMAT = (1 << 4),
    ERROR_FLASH_FULL = (1 << 5),
    ERROR_MEMORY_UNKNOWN = (1 << 6),
} errors_memory_t;

// DONE
typedef enum {
    ERROR_MCB_OK = 0,
    ERROR_MCB_SD_QUEUE_ADD = (1 << 0),
    ERROR_MCB_FLASH_QUEUE_ADD = (1 << 1),
} errors_mcb_t;

typedef enum {
    ERROR_SENSORS_OK = 0,
    ERROR_SENSOR_IMU = (1 << 0),
    ERROR_SENSOR_BAR = (1 << 1),
    ERROR_SENSOR_MAG = (1 << 2),
    ERROR_SENSOR_GPS = (1 << 3),
} errors_sensors_t;

typedef enum {
    ERROR_RECOV_OK = 0,
    ERROR_RECOV_RECEIVE = (1 << 0),
    ERROR_RECOV_TRANSMIT = (1 << 1),
} errors_recovery_t;

// DONE +-
typedef enum {
    ERROR_EXCP_OK = 0,
    ERROR_EXCP_WAKE_UP = (1 << 0),
    ERROR_EXCP_MISSION_TIMER = (1 << 1),
    ERROR_EXCP_LORA_DECODE = (1 << 2),
    ERROR_EXCP_COMMAND_NOT_FOUND = (1 << 3),
    ERROR_EXCP_NOT_ARMED = (1 << 4),
    ERROR_EXCP_STATE_CHANGE = (1 << 5),
    ERROR_EXCP_OPTION_VALUE = (1 << 6),
    ERROR_EXCP_DISCONNECT_TIMER = (1 << 7),
} errors_last_exception_t;

/**
 * @brief Initialzie configured errors
 *
 * @return true :D
 * @return false :C
 */
bool initialize_errors(void);
#endif