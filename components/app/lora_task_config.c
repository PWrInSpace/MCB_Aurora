#include "lora_task_config.h"

#include "commands_config.h"
#include "data_to_protobuf.h"
#include "errors_config.h"
#include "esp_log.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "sdkconfig.h"
#include "system_timer_config.h"
#include "utils.h"
#include "uart.h"
#include "lora_task.h"

#define TAG "LORA_C"

static bool settings_frame = false;
static uint8_t workspace[1024];

void lora_send_settings_frame(void) {
    settings_frame = true;
}

static uint16_t calculate_checksum(uint8_t* buffer, size_t size) {
    uint16_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += buffer[i];
    }

    return sum;
}

static uint8_t lora_validate(uint8_t* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        ESP_LOGE(TAG, "Invalid buffer or buffer size");
        return false;
    }

    uint8_t prefix[3];
    int rx_len = uart_read_logical(UART_LOGICAL_TELEMETRY, prefix, sizeof(prefix), portMAX_DELAY);
    if (rx_len < 3) {
        ESP_LOGE(TAG, "Invalid packet prefix");
        return false;
    }
    if (prefix[0] != PACKET_HEADER) {
        ESP_LOGE(TAG, "Invalid packet header, expected: 0x%02X, received: 0x%02X", PACKET_HEADER, prefix[0]);
        return false;
    }
    if (prefix[1] != CMD_LORA_TX) {
        ESP_LOGE(TAG, "Invalid command, expected: 0x%02X, received: 0x%02X", CMD_LORA_TX, prefix[1]);
        return false;
    }
    uint8_t data_len = prefix[2];

    // uint8_t command;
    // rx_len = uart_read_logical(UART_LOGICAL_TELEMETRY, &command, 1, pdMS_TO_TICKS(10));
    // if (command != CMD_LORA_TX || rx_len <= 0) {
    //     ESP_LOGE(TAG, "Invalid command");
    //     return false;
    // }
    //
    // uint8_t data_len;
    // rx_len = uart_read_logical(UART_LOGICAL_TELEMETRY, &data_len, 1, pdMS_TO_TICKS(10));
    // if (data_len == 0 || data_len > buffer_size || rx_len <= 0) {
    //     ESP_LOGE(TAG, "Data length is too big, max: %d, received: %d", buffer_size, data_len);
    //     return false;+3
    // }

    rx_len = uart_read_logical(UART_LOGICAL_TELEMETRY, buffer, data_len, portMAX_DELAY);
    if (rx_len != data_len) {
        ESP_LOGE(TAG, "Data length mismatch, expected: %d, received: %d", data_len, rx_len);
        return false;
    }

    uint16_t checksum;
    rx_len = uart_read_logical(UART_LOGICAL_TELEMETRY, (uint8_t*)&checksum, 2, portMAX_DELAY);
    if (rx_len != 2) {
            ESP_LOGE(TAG, "Checksum length mismatch, expected: 2, received: %d", rx_len);
            return false;
    }
    //todo dodać walidację checksum

    return data_len;
}

static void lora_process(uint8_t* packet, size_t packet_size) {
    if (packet_size > 40) {
        ESP_LOGE(TAG, "Received packet is too big");
        errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_LORA_DECODE, 100);
        return;
    }

    struct obc_lo_ra_command_t* received = obc_lo_ra_command_new(&workspace, sizeof(workspace));

    size_t decoded_size = 0;
    decoded_size = obc_lo_ra_command_decode(received, packet, packet_size);
    if (decoded_size > 0 &&
        received->lora_dev_id.is_present &&
        received->sys_dev_id.is_present &&
        received->command.is_present &&
        received->payload.is_present) {
        cmd_message_t received_command = cmd_create_message(received->command.value, received->payload.value);

        ESP_LOGI(TAG, "Received command from LoRa -> lora_dev_id: %d, sys_dev_id: %d, command: %d, payload: %d",
            received->lora_dev_id.value, received->sys_dev_id.value, received->command.value, received->payload.value);

        if (lora_cmd_process_command(received->lora_dev_id.value, received->sys_dev_id.value, &received_command) == false) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_COMMAND_NOT_FOUND, 200);
            ESP_LOGE(TAG, "Unable to process command :C");
            return;
        }
    } else {
        ESP_LOGE(TAG, "Unable to decode received package");
    }

    if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
        ESP_LOGE(TAG, "Unable to restart timer");
    }
}

static size_t add_packet_info(uint8_t* buffer, size_t size, uint8_t data_size, obc_com_command_t command) {
    size_t info_size = sizeof(PACKET_HEADER) + sizeof(command) + DATA_SIZE_LEN + CHECKSUM_LEN;
    // dostępne miejsce
    if (size - data_size < info_size) return 0;

    uint16_t checksum = calculate_checksum(buffer, data_size + info_size - CHECKSUM_LEN);

    buffer[0] = PACKET_HEADER;
    buffer[sizeof(PACKET_HEADER)] = command;
    buffer[sizeof(PACKET_HEADER) + sizeof(command)] = data_size;
    buffer[sizeof(PACKET_HEADER) + sizeof(command) + DATA_SIZE_LEN + data_size] = checksum & 0xFF;
    buffer[sizeof(PACKET_HEADER) + sizeof(command) + DATA_SIZE_LEN + data_size + 1] = checksum >> 8 & 0xFF;

    return info_size;
}

static size_t lora_create_settings_packet(uint8_t* buffer, size_t size) {
    /* create settings protobuf into buffer, reserve 1 byte at the end for checksum */
    if (buffer == NULL || size == 0) return 0;

    size_t info_size = sizeof(PACKET_HEADER) + sizeof(CMD_LORA_TX) + DATA_SIZE_LEN + CHECKSUM_LEN;

    struct obc_lo_ra_settings_t *frame = obc_lo_ra_settings_new(&workspace, sizeof(workspace));
    create_protobuf_settings_frame(frame);
    size_t data_size = obc_lo_ra_settings_encode(frame, buffer + sizeof(PACKET_HEADER) + sizeof(CMD_LORA_TX) + DATA_SIZE_LEN, size - info_size);
    if (data_size == 0) return 0;

    if (info_size != add_packet_info(buffer, size, data_size, CMD_LORA_TX)) return 0;

    return info_size + data_size;
}

static size_t lora_create_data_packet(uint8_t* buffer, size_t size) {
    /* create data protobuf into buffer, reserve 1 byte at the end for checksum */
    if (buffer == NULL || size == 0) return 0;

    size_t info_size = sizeof(PACKET_HEADER) + sizeof(CMD_LORA_TX) + DATA_SIZE_LEN + CHECKSUM_LEN;

    struct obc_lo_ra_frame_t *frame = obc_lo_ra_frame_new(&workspace, sizeof(workspace));
    create_protobuf_data_frame(frame);
    size_t data_size = obc_lo_ra_frame_encode(frame, buffer + sizeof(PACKET_HEADER) + sizeof(CMD_LORA_TX) + DATA_SIZE_LEN, size - info_size);
    if (data_size == 0) return 0;

    if (info_size != add_packet_info(buffer, size, data_size, CMD_LORA_TX)) return 0;

    return info_size + data_size;
}

static size_t lora_packet(uint8_t* buffer, size_t buffer_size) {
    size_t size = 0;

    if (settings_frame == true) {
        size = lora_create_settings_packet(buffer, buffer_size);
        settings_frame = false;
        //ESP_LOGI(TAG, "Transmitting settings frame");
    } else {
        size = lora_create_data_packet(buffer, buffer_size);
    }

    ESP_LOGI(TAG, "Sending LoRa frame -> size: %d", size);

    return size;
}

bool initialize_lora(uint32_t frequency_khz, uint32_t transmitting_period) {
    // RETURN_ON_FALSE(lora_hw_spi_add_device(VSPI_HOST));
    // RETURN_ON_FALSE(lora_hw_set_gpio());
    // RETURN_ON_FALSE(lora_hw_attach_d0_interrupt(lora_task_irq_notify));
    // lora_struct_t lora = {._spi_transmit = lora_hw_spi_transmit,
    //                       ._delay = lora_hw_delay,
    //                       ._gpio_set_level = lora_hw_gpio_set_level,
    //                       .log = lora_hw_log,
    //                       .rst_gpio_num = CONFIG_LORA_RS,
    //                       .cs_gpio_num = CONFIG_LORA_CS,
    //                       .d0_gpio_num = CONFIG_LORA_D0,
    //                       .implicit_header = 0,
    //                       .frequency = 0};
    lora_api_config_t cfg = {
        .validate_rx_packet_fnc = lora_validate,
        .process_rx_packet_fnc = lora_process,
        .get_tx_packet_fnc = lora_packet,
        .frequency_khz = frequency_khz,
        .transmitting_period = transmitting_period,
    };

    RETURN_ON_FALSE(uart_init_logical(UART_LOGICAL_TELEMETRY, UART_NUM_1, LORA_UART_RX, LORA_UART_TX, LORA_UART_BAUDRATE));
    uart_write_logical(UART_LOGICAL_TELEMETRY, (uint8_t*) "LORA INIT", 10);
    RETURN_ON_FALSE(lora_task_init(&cfg));
    return true;
}