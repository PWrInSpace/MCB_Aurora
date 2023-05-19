// Copyright 2022 PWrInSpace, Kuba
#ifndef UBLOX_M8_HH
#define UBLOX_M8_HH

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define UBX_READ_BUFFER_SIZE 255

typedef int (*ublox_m8_uart_write)(uint8_t *data, uint8_t data_size);
typedef int (*ublox_m8_uart_read)(uint8_t *data, uint8_t data_size);

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
    ublox_m8_uart_read uart_read_fnc;
    ublox_m8_uart_write uart_write_fnc;
    uint8_t read_buffer[UBX_READ_BUFFER_SIZE];
    uint8_t read_data_size;
} ublox_m8_t;


bool ublox_m8_init(ublox_m8_t *ubx);

bool ublox_m8_get_PVT(ublox_m8_t *ubx, ublox_m8_pvt_t *pvt);

#endif

