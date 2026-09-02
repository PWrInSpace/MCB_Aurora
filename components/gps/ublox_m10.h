// Copyright 2022 PWrInSpace, Kuba
#ifndef UBLOX_M10_H
#define UBLOX_M10_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define UBX_MESSAGE_CLASS_POSITION 2
#define UBX_MESSAGE_ID_POSITION 3

#define UBX_INVALID_CLASS 0x00
#define UBX_INVALID ID 0x00

#define UBX_READ_BUFFER_SIZE 255
#define UBX_SEND_BUFFER_SIZE 100

#define UBX_REQUEST_LENGTH 0x00
#define UBX_REQUEST_PAYLOAD 0x00

#define UBX_SYNC_CHAR_1 0xB5
#define UBX_SYNC_CHAR_2 0x62

#define UBX_CLASS_NAV 0x01
#define UBX_NAV_ID_PVT 0x07

#define UBX_CLASS_ACK 0x05
#define UBX_ACK_ID_NAK 0x00
#define UBX_ACK_ID_ACK 0x01

#define UBX_CLASS_CFG 0x06
#define UBX_CFG_ID_NAV5 0x24
#define UBX_CFG_ID_ESFLAG 0x56


typedef int (*ublox_m10_uart_write)(uint8_t *data, uint8_t data_size);
typedef int (*ublox_m10_uart_read)(uint8_t *data, uint8_t data_size);
typedef void (*ublox_m10_delay)(uint32_t milliseconds);

// --- Konfiguracja GPS ---
static uint8_t configUBX[]          = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9A, 0x79};
static uint8_t disableNmeaAll[]     = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x9D, 0xDF};
static uint8_t disableInfMessages[] = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x92, 0x20, 0x00, 0xB0, 0x63};
static uint8_t setRocketMode4G[]    = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x21, 0x00, 0x11, 0x20, 0x08, 0x94, 0xB7};
static uint8_t enableNavPvt[]       = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x18, 0xE1};
static uint8_t setRate1Hz_M10[]     = {0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x21, 0x30, 0xE8, 0x03, 0xF1, 0xAE};

typedef struct {
    int32_t lon;
    int32_t lat;
    int32_t height;
    uint8_t numSV;
    uint8_t fix_type;
} ublox_m10_pvt_t;

typedef struct {
    uint32_t yaw;
    int16_t pitch;
    int16_t roll;
} ublox_m10_esfalg_t;

typedef enum {
    PORTABLE = 0,
    STATIONARY = 2,
    PEDESTRIAN = 3,
    AUTOMOTIVE = 4,
    SEA = 5,
    AIRBORNE_1G = 6,
    AIRBORNE_2G = 7,
    AIRBORNE_4G = 8,
    FIRST_WORN_WATCH = 9,
    MOTORBIKE = 10,
    ROBOTIC_LAWN_MOWER = 11,
    ELECTRIC_KICK_SCOOTER = 12,
} ublox_m8_dynamic_model_t;

/**
 * @brief Ublox m8 handler, must be initialized as global variable
 */
typedef struct {
    ublox_m10_uart_read uart_read_fnc;
    ublox_m10_uart_write uart_write_fnc;
    ublox_m10_delay delay_fnc;
    uint8_t read_buffer[UBX_READ_BUFFER_SIZE];
    uint8_t send_buffer[UBX_SEND_BUFFER_SIZE];
    uint8_t read_data_size;
} ublox_m10_t;

/**
 * @brief Initalize ublox
 * 
 * @param ubx pointer to ubx
 * @return true :D
 * @return false :C
 */
bool ublox_m10_init(ublox_m10_t *ubx);

/**
 * @brief Get PVT data - positioning data
 * 
 * @param ubx pointer to ubx struct
 * @param pvt pointer to save data
 * @return true :D
 * @return false :C
 */
bool ublox_m10_get_PVT(ublox_m10_t *ubx, ublox_m10_pvt_t *pvt);

#endif

