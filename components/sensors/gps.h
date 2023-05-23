// Copyright 2022 PWrInSpace, Kuba
#ifndef UBLOX_M8_HH
#define UBLOX_M8_HH

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define UBX_MESSAGE_CLASS_POSITION 2
#define UBX_MESSAGE_ID_POSITION 3

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


typedef int (*ublox_m8_uart_write)(uint8_t *data, uint8_t data_size);
typedef int (*ublox_m8_uart_read)(uint8_t *data, uint8_t data_size);
typedef void (*ublox_m8_delay)(uint32_t miliseconds);

typedef uint8_t ublox_u1_t;
typedef int8_t ublox_i1_t ;

typedef union {
    uint8_t raw[2];
    uint16_t data;
} ublox_u2_t;

typedef union {
    uint8_t raw[2];
    int16_t data;
} ublox_i2_t;

typedef union {
    uint8_t raw[4];
    uint32_t data;
} ublox_u4_t;

typedef union {
    uint8_t raw[4];
    int32_t data;
} ublox_i4_t;

typedef struct {
    ublox_i4_t lon;
    ublox_i4_t lat;
    ublox_i4_t height;
    ublox_u1_t numSV;
    ublox_u1_t fix_type;
} ublox_m8_pvt_t;

typedef struct {
    ublox_u4_t yaw;
    ublox_i2_t pitch;
    ublox_i2_t roll;
} ublox_m8_esfalg_t;

typedef enum {
    PORTABLE = 0,
    STATIONARY = 2,
    PEDESTRIAN = 3,
    AUTOMOTIVE = 4,
    SEA = 5,
    AIRBONRE_1G = 6,
    AIRBORNE_2G = 7,
    AIRBORNE_4G = 8,
    WIRST_WORN_WATCH = 9,
    MOTORBIKE = 10,
    ROBOTIC_LAWN_MOWER = 11,
    ELECTRIC_KICK_SCOOTER = 12,
} ublox_m8_dynamic_model_t;

/**
 * @brief Ublox m8 handler, must be initialized as global variable
 * 
 */
typedef struct {
    ublox_m8_uart_read uart_read_fnc;
    ublox_m8_uart_write uart_write_fnc;
    ublox_m8_delay delay_fnc;
    uint8_t read_buffer[UBX_READ_BUFFER_SIZE];
    uint8_t send_buffer[UBX_SEND_BUFFER_SIZE];
    uint8_t read_data_size;
} ublox_m8_t;

bool ublox_m8_init(ublox_m8_t *ubx);

// bool ublox_m8_set_dynamic_model(ublox_m8_t *ubx, ublox_m8_dynamic_model_t dyn_model);

bool ublox_m8_get_PVT(ublox_m8_t *ubx, ublox_m8_pvt_t *pvt);

// bool ublox_m8_get_ESFALG(ublox_m8_t *ubx, ublox_m8_esfalg_t *esfalg);

#endif

