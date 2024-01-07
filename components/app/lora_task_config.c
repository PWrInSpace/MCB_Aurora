#include "lora_task_config.h"
#
#include "commands_config.h"
#include "data_to_protobuf.h"
#include "errors_config.h"
#include "esp_log.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "rocket_data.h"
#include "sdkconfig.h"
#include "system_timer_config.h"
#include "utils.h"

#define TAG "LORA_C"
#define PREFIX "B"

static lora_struct_t lora = {._spi_transmit = lora_hw_spi_transmit,
                             ._delay = lora_hw_delay,
                             ._gpio_set_level = lora_hw_gpio_set_level,
                             .log = lora_hw_log,
                             .rst_gpio_num = CONFIG_LORA_RS,
                             .cs_gpio_num = CONFIG_LORA_CS,
                             .d0_gpio_num = CONFIG_LORA_D0,
                             .implicit_header = 0,
                             .frequency = 0};

static bool settings_frame = false;
void lora_send_settings_frame(void) { settings_frame = true; }

static bool check_prefix(uint8_t* packet, size_t packet_size) {
    if (packet_size < sizeof(PACKET_PREFIX)) {
        return false;
    }

    uint8_t prefix[] = PACKET_PREFIX;
    for (int i = 0; i < sizeof(PACKET_PREFIX) - 1; ++i) {
        if (packet[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}

static uint8_t calculate_checksum(uint8_t* buffer, size_t size) {
    uint8_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += buffer[i];
    }

    return sum;
}

#define POLYNOMIAL 0x31

uint8_t calculate_crc(const uint8_t* data, size_t size) {
    uint8_t crc = 0;

    for (size_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
        }
    }
    return crc;
}

int check_crc(const uint8_t* buffer, size_t size_buffer) {
    if (size_buffer < 1) {
        return 1;
    }
    uint8_t received_crc = buffer[size_buffer - 1];

    uint8_t calculated_crc = calculate_crc(buffer, size_buffer - 1);

    if (calculated_crc == received_crc) {
        return 0;
    } else {
        return 1;
    }
}

struct Packet {
    float latitude;
    float longitude;
    float gps_altitude;
    uint8_t received_counter;
    uint8_t missrecieved_counter;
    int16_t RSSI;
    int16_t SNR;
} packet;

static struct Packet packet_data = {.latitude = 0,
                                    .longitude = 0,
                                    .gps_altitude = 0,
                                    .received_counter = 0,
                                    .missrecieved_counter = 0,
                                    .RSSI = 0,
                                    .SNR = 0};
static void lora_process(uint8_t* packet, size_t packet_size) {
    // if(!calculate_checksum(packet_data, packet_size))
    packet_data.SNR = lora_packet_snr(&lora);
    packet_data.RSSI = lora_packet_rssi(&lora);
    if (!calculate_checksum(packet, packet_size)) {
        packet_data.missrecieved_counter++;
        // JAK DOBRAC SIE DO LORY
        return;
    }
    packet_data.received_counter++;

    return;
}

// LITLE ENDIAN IN ESP32

static size_t lora_packet(uint8_t* buffer, size_t buffer_size) {
    uint8_t* tmp_buff_size = buffer;
    mcb_data_t data = rocket_data_get_mcb();
    packet_data.latitude = data.latitude;
    packet_data.longitude = data.longitude;
    packet_data.gps_altitude = data.gps_altitude;

    size_t size_payload = sizeof(struct Packet);
    if (buffer_size < size_payload) {
        return 0;
    }
    memcpy(buffer, PREFIX, sizeof(PREFIX) - 1);
    buffer += sizeof(PREFIX);
    memcpy(buffer, &packet_data.latitude, sizeof(packet_data.latitude));
    buffer += sizeof(packet_data.latitude);
    memcpy(buffer, &packet_data.longitude, sizeof(packet_data.longitude));
    buffer += sizeof(packet_data.longitude);
    memcpy(buffer, &packet_data.gps_altitude, sizeof(packet_data.gps_altitude));
    buffer += sizeof(packet_data.gps_altitude);
    memcpy(buffer, &packet_data.received_counter, sizeof(packet_data.received_counter));
    buffer += sizeof(packet_data.received_counter);
    memcpy(buffer, &packet_data.RSSI, sizeof(packet_data.RSSI));
    buffer += sizeof(packet_data.RSSI);
    memcpy(buffer, &packet_data.SNR, sizeof(packet_data.SNR));
    buffer += sizeof(packet_data.SNR);
    size_payload = buffer - tmp_buff_size;

    return size_payload;
}

static size_t add_prefix(uint8_t* buffer, size_t size) {
    if (size < 6) {
        return 0;
    }

    memcpy(buffer, PACKET_PREFIX, sizeof(PACKET_PREFIX) - 1);

    return sizeof(PACKET_PREFIX) - 1;
}

static size_t lora_create_settings_packet(uint8_t* buffer, size_t size) {
    LoRaSettings frame = LO_RA_SETTINGS__INIT;
    create_protobuf_settings_frame(&frame);

    uint8_t data_size = 0;
    uint8_t prefix_size = 0;
    prefix_size = add_prefix(buffer, size);
    data_size = lo_ra_settings__pack(&frame, buffer + prefix_size);

    return prefix_size + data_size;
}

static size_t lora_create_data_packet(uint8_t* buffer, size_t size) {
    LoRaFrame frame;
    create_porotobuf_data_frame(&frame);

    uint8_t data_size = 0;
    uint8_t prefix_size = 0;
    prefix_size = add_prefix(buffer, size);
    data_size = lo_ra_frame__pack(&frame, buffer + prefix_size);

    return prefix_size + data_size;
}

// static size_t lora_packet(uint8_t* buffer, size_t buffer_size) {
//     size_t size = 0;

//     if (settings_frame == true) {
//         size = lora_create_settings_packet(buffer, buffer_size);
//         settings_frame = false;
//         ESP_LOGI(TAG, "Transmiting settings frame");
//     } else {
//         size = lora_create_data_packet(buffer, buffer_size);
//     }

//     ESP_LOGI(TAG, "Sending LoRa frame -> size: %d", size);

//     return size;
// }

bool initialize_lora(uint32_t frequency_khz, uint32_t transmiting_period) {
    RETURN_ON_FALSE(lora_hw_spi_add_device(VSPI_HOST));
    RETURN_ON_FALSE(lora_hw_set_gpio());
    RETURN_ON_FALSE(lora_hw_attach_d0_interrupt(lora_task_irq_notify));

    lora_api_config_t cfg = {
        .lora = &lora,
        .process_rx_packet_fnc = lora_process,
        .get_tx_packet_fnc = lora_packet,
        .frequency_khz = frequency_khz,
        .transmiting_period = transmiting_period,
    };
    RETURN_ON_FALSE(lora_task_init(&cfg));
    return true;
}