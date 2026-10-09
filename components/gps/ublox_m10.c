// Copyright 2022 PWrInSpace, Kuba
#include "ublox_m10.h"
#include "esp_log.h"
#include "assert.h"
#include <stdio.h>
#include <memory.h>

#define TAG "GPS"

/**
 * @brief Calculate ublox checksum
 *
 * @param buffer message buffer
 * @param message_size message size without checksum
 * @param CK_A checksum a
 * @param CK_B checksum b
 */
static void calculate_checksum(uint8_t* buffer, uint8_t message_size, uint8_t *CK_A, uint8_t *CK_B) {
    uint8_t checksum_a = 0;
    uint8_t checksum_b = 0;

    for (int i = 2; i < message_size; ++i) {
        checksum_a = checksum_a + buffer[i];
        checksum_b = checksum_b + checksum_a;
    }

    *CK_A = checksum_a;
    *CK_B = checksum_b;
}

/**
 * @brief Create a request message object
 *
 * @param buffer buffer to store message
 * @param ubx_class ublox message class
 * @param ubx_id ublox message id
 * @return uint8_t size of message
 */
uint8_t create_request_message(uint8_t *buffer, uint8_t ubx_class, uint8_t ubx_id) {
    uint8_t message_size = 0;
    buffer[message_size++] = UBX_SYNC_CHAR_1;
    buffer[message_size++] = UBX_SYNC_CHAR_2;
    buffer[message_size++] = ubx_class;
    buffer[message_size++] = ubx_id;
    buffer[message_size++] = UBX_REQUEST_LENGTH;
    buffer[message_size++] = UBX_REQUEST_LENGTH;

    uint8_t ck_a;
    uint8_t ck_b;
    calculate_checksum(buffer, message_size, &ck_a, &ck_b);
    buffer[message_size++] = ck_a;
    buffer[message_size++] = ck_b;

    return message_size;
}

bool check_received_message(uint8_t *received, uint8_t length) {
    if (length < 8) {
        return false;
    }

    // UBX return NACK
    if (received[UBX_MESSAGE_CLASS_POSITION] == UBX_CLASS_ACK &&
        received[UBX_MESSAGE_ID_POSITION] == UBX_ACK_ID_NAK) {
        return false;
    }

    uint8_t ck_a;
    uint8_t ck_b;
    calculate_checksum(received, length - 2, &ck_a, &ck_b);
    // Invalid checksum
    if (ck_a != received[length - 2] || ck_b != received[length - 1]) {
        return false;
    }

    return true;
}

bool ublox_m10_init(ublox_m10_t *ubx) {
    if (ubx->uart_read_fnc == NULL || ubx->uart_write_fnc == NULL || ubx->delay_fnc == NULL) {
        return false;
    }

    ESP_LOGI(TAG, "Inicjalizacja u-blox M10 (Rocket Mode, 1Hz, PVT)...");

    if (ubx->uart_write_fnc(configUBX, sizeof(configUBX)) != sizeof(configUBX)) {
        return false;
    }

    ubx->delay_fnc(50);
    ubx->uart_write_fnc(disableInfMessages, sizeof(disableInfMessages));
    ubx->delay_fnc(50);
    ubx->uart_write_fnc(disableNmeaAll, sizeof(disableNmeaAll));
    ubx->delay_fnc(50);
    ubx->uart_write_fnc(setRocketMode4G, sizeof(setRocketMode4G));
    ubx->delay_fnc(50);
    // ubx->uart_write_fnc(enableNavPvt, sizeof(enableNavPvt));
    // ubx->delay_fnc(50);
    ubx->uart_write_fnc(setRate1Hz_M10, sizeof(setRate1Hz_M10));
    ubx->delay_fnc(100);
    return true;
}

bool ublox_m10_get_PVT(ublox_m10_t *ubx, ublox_m10_pvt_t *pvt) {
    uint8_t message_size = create_request_message(
        ubx->send_buffer, UBX_CLASS_NAV, UBX_NAV_ID_PVT);

    if (ubx->uart_write_fnc(ubx->send_buffer, message_size) != message_size) {
        return false;
    }

    ubx->read_data_size = ubx->uart_read_fnc(ubx->read_buffer, sizeof(ubx->read_buffer));

    if (check_received_message(ubx->read_buffer, ubx->read_data_size) == false) {
        return false;
    }

    pvt->fix_type = ubx->read_buffer[26];
    pvt->numSV = ubx->read_buffer[29];
    memcpy(&pvt->lon, &ubx->read_buffer[30], sizeof(pvt->lon));
    memcpy(&pvt->lat, &ubx->read_buffer[34], sizeof(pvt->lat));
    memcpy(&pvt->height, &ubx->read_buffer[38], sizeof(pvt->height));

    return true;
}