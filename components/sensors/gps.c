// Copyright 2022 PWrInSpace, Kuba
#include "gps.h"
#include "esp_log.h"
#include "assert.h"
#include <stdio.h>
#include <memory.h>

#define TAG "GPS"


static uint8_t configUBX[]={0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x80,0x25,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x9A,0x79};
static uint8_t getPVTData[]={0xB5,0x62,0x01,0x07,0x00,0x00,0x08,0x19};


bool ublox_m8_init(ublox_m8_t *ubx) {
    assert(ubx->uart_read_fnc != NULL);
    assert(ubx->uart_write_fnc != NULL);
    if (ubx->uart_read_fnc == NULL || ubx->uart_write_fnc == NULL) {
        return false;
    }

    if (ubx->uart_write_fnc(configUBX, sizeof(configUBX)) !=  sizeof(configUBX)) {
        return false;
    }

    memset(ubx->read_buffer, 0, sizeof(ubx->read_buffer));
    ubx->read_data_size = 0;

    return true;
}

bool ublox_m8_get_PVT(ublox_m8_t *ubx, ublox_m8_pvt_t *pvt) {
    if (ubx->uart_write_fnc(getPVTData, sizeof(getPVTData)) != sizeof(getPVTData)) {
        return false;
    }

    ubx->read_data_size = ubx->uart_read_fnc(ubx->read_buffer, sizeof(ubx->read_buffer));

    if (ubx->read_data_size < 1) {
        return false;
    }

    pvt->fix_type = ubx->read_buffer[26];
    pvt->numSV = ubx->read_buffer[29];
    memcpy(pvt->lon.raw, &ubx->read_buffer[30], sizeof(pvt->lon));
    memcpy(pvt->lat.raw, &ubx->read_buffer[34], sizeof(pvt->lat));
    memcpy(pvt->height.raw, &ubx->read_buffer[38], sizeof(pvt->height));

    return true;
}