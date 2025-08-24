// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_now_config.h"
#include "esp_log.h"
#include "esp_now_api.h"
#include "rocket_data.h"
#include "state_machine.h"
#include "errors_config.h"

#define TAG "ENC"

static esp_now_connected_devices_t connected;

static void callback_pitot(uint8_t *data, size_t size);
static void callback_servo_n2o (uint8_t *data, size_t size);
static void callback_servo_eth_n2(uint8_t *data, size_t size);
static void callback_sol_n2o_n2(uint8_t *data, size_t size);
static void callback_sol_eth(uint8_t *data, size_t size);
static void callback_tanwa(uint8_t *data, size_t size);
static void callback_payload(uint8_t *data, size_t size);

const ENA_device_t esp_now_broadcast = {
    .peer = {.peer_addr = BROADCAST_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = NULL,
};

const ENA_device_t esp_now_pitot = {
    .peer = {.peer_addr = PITOT_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_pitot,
};


const ENA_device_t esp_now_servo_n2o = {
    .peer = {.peer_addr = SERVO_N2O_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_servo_n2o,
};

const ENA_device_t esp_now_servo_eth_n2 = {
    .peer = {.peer_addr = SERVO_ETH_N2_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_servo_eth_n2,
};

const ENA_device_t esp_now_sol_n2o_n2 = {
    .peer = {.peer_addr = SOL_N2O_N2_MAC .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_sol_n2o_n2,
};

const ENA_device_t esp_now_sol_eth = {
    .peer = {.peer_addr = SOL_ETH_MAC .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_sol_eth,
};

const ENA_device_t esp_now_tanwa = {
    .peer = {.peer_addr = TANWA_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_tanwa,
};

const ENA_device_t esp_now_payload = {
    .peer = {.peer_addr = PAYLOAD_MAC, .channel = ESP_NOW_CHANNEL},
    .on_receive = callback_payload,
};

static void callback_pitot(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Pitot receive, size %d", size);
    connected.pitot = true;
    if (size == sizeof(pitot_data_t)) {
        rocket_data_update_pitot((pitot_data_t *) data);
    }
}


static void callback_servo_n2o(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Servo N2O receive, size %d", size);
    connected.servo_n2o = true;
    if (size == sizeof(servo_n2o_data_t)) {
        rocket_data_update_servo_n2o((servo_n2o_data_t *) data);
    }
}

static void callback_servo_eth_n2(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Servo Ethanol and N2 receive, size %d", size);
    connected.servo_eth_n2 = true;
    if (size == sizeof(servo_eth_n2_data_t)) {
        rocket_data_update_servo_eth_n2((servo_eth_n2_data_t *) data);
    }
}

static void callback_sol_n2o_n2(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Solenoid N2O and N2 receive, size %d", size);
    connected.sol_n2o = true;
    if (size == sizeof(sol_n2o_n2_data_t)) {
        rocket_data_update_sol_n2o((sol_n2o_n2_data_t *) data);
    }
}

static void callback_sol_eth(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Solenoid Ethanol receive, size %d", size);
    connected.sol_eth = true;
    if (size == sizeof(sol_eth_data_t)) {
        rocket_data_update_sol_eth((sol_eth_data_t *) data);
    }
}

static void callback_tanwa(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Tanwa receive, size %d", size);
    connected.tanwa = true;
    if (size == sizeof(tanwa_data_t)) {
        rocket_data_update_tanwa((tanwa_data_t *) data);
    }
}

static void callback_payload(uint8_t *data, size_t size) {
    ESP_LOGI(TAG, "Received from payload 0x%2X, size %d", data[0], size);
    connected.payload = true;
    if (size == sizeof(payload_data_t)) {
        rocket_data_update_payload((payload_data_t *) data);
    } else {
        ESP_LOGE(TAG, "Payload invalid size");
    }
}

static void temp_on_error(ENA_ERROR error) {
    error_code_t err_code;
    if (error == ENA_SEND || error == ENA_SEND_CB_QUEUE || error == ENA_SEND_REPLY) {
        err_code = ERROR_ESP_NOW_SEND;
    } else if (error == ENA_REC || error == ENA_REC_CB_QUEUE) {
        err_code = ERROR_ESP_NOW_RECEIVE;
    } else if (error == ENA_DEVICES_NULL || error == ENA_UNKNOWN_DEV) {
        err_code = ERROR_ESP_NOW_DEVICE;
    } else {
        err_code = ERROR_ESP_UNKNOWN;
    }

    ESP_LOGE(TAG, "!!! ESP NOW ERROR, CODE: %d !!!", error);
    errors_add(ERROR_TYPE_ESP_NOW, err_code, 200);
}

// TO DO add mutex, not critical so i do not make this
void esp_now_get_connected_dev(esp_now_connected_devices_t *dev) {
    *dev = connected;
}

void esp_now_clear_connected_dev(void) {
    memset(&connected, 0, sizeof(connected));
}

bool initialize_esp_now(void) {
    esp_err_t status = ESP_OK;
    uint8_t mac_address[] = MCB_MAC;
    ENA_config_t cfg = {
      .stack_depth = ESP_NOW_TASK_STACK_DEPTH,
      .priority = ESP_NOW_TASK_PRIORITY,
      .core_id = ESP_NOW_TASK_CORE_ID,
    };

    status |= ENA_init(mac_address);
    status |= ENA_register_device(&esp_now_broadcast);
    status |= ENA_register_device(&esp_now_pitot);
    status |= ENA_register_device(&esp_now_payload);
    status |= ENA_register_device(&esp_now_servo_eth_n2);
    status |= ENA_register_device(&esp_now_servo_n2o);
    status |= ENA_register_device(&esp_now_sol_n2o_n2);
    status |= ENA_register_device(&esp_now_sol_eth);
    status |= ENA_register_device(&esp_now_tanwa);
    status |= ENA_register_error_handler(temp_on_error);
    status |= ENA_run(&cfg);

    return status == ESP_OK ? true : false;
}
