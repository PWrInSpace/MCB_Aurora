// Copyright 2022 PWrInSpace, Kuba
#include "ublox_m8.h"
#include "esp_log.h"
#include "assert.h"
#include <stdio.h>
#include <memory.h>

#define TAG "GPS"


static uint8_t configUBX[]={0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x80,0x25,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x9A,0x79};

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

bool ublox_m8_init(ublox_m8_t *ubx) {
    assert(ubx->uart_read_fnc != NULL);
    assert(ubx->uart_write_fnc != NULL);
    if (ubx->uart_read_fnc == NULL || ubx->uart_write_fnc == NULL) {
        return false;
    }

    if (ubx->uart_write_fnc(configUBX, sizeof(configUBX)) !=  sizeof(configUBX)) {
        return false;
    }

    return true;
}

bool ublox_m8_get_PVT(ublox_m8_t *ubx, ublox_m8_pvt_t *pvt) {
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
    memcpy(pvt->lon.raw, &ubx->read_buffer[30], sizeof(pvt->lon));
    memcpy(pvt->lat.raw, &ubx->read_buffer[34], sizeof(pvt->lat));
    memcpy(pvt->height.raw, &ubx->read_buffer[38], sizeof(pvt->height));

    return true;
}

// bool ublox_m8_set_dynamic_model(ublox_m8_t *ubx, ublox_m8_dynamic_model_t dyn_model) {
//     uint8_t message_size = create_request_message(
//         ubx->send_buffer, UBX_CLASS_CFG, UBX_CFG_ID_NAV5);
//     ubx->read_data_size = ubx->uart_read_fnc(ubx->read_buffer, sizeof(ubx->read_buffer));
//     if (ubx->uart_write_fnc(ubx->send_buffer, message_size) != message_size) {
//         return false;
//     }

//     ubx->read_data_size = ubx->uart_read_fnc(ubx->read_buffer, sizeof(ubx->read_buffer));

//     if (check_received_message(ubx->read_buffer, ubx->read_data_size) == false) {
//         ESP_LOGE(TAG, "Receive error");
//         return false;
//     }
//     ESP_LOGI(TAG, "DYNAMIC MODEL %d", ubx->read_buffer[8]);
//     ubx->read_buffer[8] = dyn_model;

//     if (ubx->uart_write_fnc(ubx->read_buffer, ubx->read_data_size) != message_size) {
//         return false;
//     }

//     return true;
// }

// bool ublox_m8_get_ESFALG(ublox_m8_t *ubx, ublox_m8_esfalg_t *esfalg) {
//     uint8_t message_size = create_request_message(
//         ubx->send_buffer, UBX_CLASS_CFG, UBX_CFG_ID_ESFLAG);

//     if (ubx->uart_write_fnc(ubx->send_buffer, message_size) != message_size) {
//         return false;
//     }

//     ubx->read_data_size = ubx->uart_read_fnc(ubx->read_buffer, sizeof(ubx->read_buffer));

//     if (ubx->read_data_size < 1) {
//         return false;
//     }

//     if (check_received_message(ubx->read_buffer, ubx->read_data_size) == false) {
//         return false;
//     }

//     memcpy(esfalg->yaw.raw, &ubx->read_buffer[10], sizeof(esfalg->yaw));
//     memcpy(esfalg->pitch.raw, &ubx->read_buffer[14], sizeof(esfalg->pitch));
//     memcpy(esfalg->roll.raw, &ubx->read_buffer[16], sizeof(esfalg->roll));
//     ESP_LOGI(TAG, "YAW %d", esfalg->yaw.data);
//     ESP_LOGI(TAG, "PITCH %d", esfalg->pitch.data);
//     ESP_LOGI(TAG, "ROLL %d", esfalg->roll.data);
//     return true;
// }