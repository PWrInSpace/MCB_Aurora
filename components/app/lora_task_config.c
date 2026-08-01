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

#define TAG "LORA_C"

static bool settings_frame = false;
static uint8_t workspace[1024];

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

static void lora_process(uint8_t* packet, size_t packet_size) {
    ESP_LOGI(TAG, "Received packet");
    if (check_prefix(packet, packet_size) == false) {
        ESP_LOGE(TAG, "LoRa invalid prefix");
        return;
    }

    uint8_t prefix_size = sizeof(PACKET_PREFIX) - 1;
    if (calculate_checksum(packet + prefix_size, packet_size - prefix_size - 1) != packet[packet_size - 1]) {
        ESP_LOGE(TAG, "Invalid checksum");
        return;
    }

    struct obc_lo_ra_frame_t* received = obc_lo_ra_frame_new(&workspace, sizeof(workspace));

    size_t decoded_size = obc_lo_ra_frame_decode(received, packet + prefix_size, packet_size - prefix_size - 1);

    ESP_LOGI(TAG, "Received frame type: %d", received->frame);

    if (received->frame != obc_lo_ra_frame_frame_app_frame_e) {
        ESP_LOGE(TAG, "Received frame is not an app frame");
        return;
    }

    if (decoded_size > 0 && received->app_frame_p->lora_dev_id.is_present && received->app_frame_p->sys_dev_id.is_present && received->app_frame_p->command.is_present && received->app_frame_p->payload.is_present) {
        cmd_message_t received_command = cmd_create_message(received->app_frame_p->command.value, received->app_frame_p->payload.value);
        ESP_LOGI(TAG, "Received command from LoRa -> lora_dev_id: %d, sys_dev_id: %d, command: %d, payload: %d", received->app_frame_p->lora_dev_id.value, received->app_frame_p->sys_dev_id.value, received->app_frame_p->command.value, received->app_frame_p->payload.value);

        if (lora_cmd_process_command(received->app_frame_p->lora_dev_id.value, received->app_frame_p->sys_dev_id.value, &received_command) == false) {
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

static size_t add_prefix(uint8_t* buffer, size_t size) {
    if (size < 6) {
        return 0;
    }

    memcpy(buffer, PACKET_PREFIX, sizeof(PACKET_PREFIX) - 1);

    return sizeof(PACKET_PREFIX) - 1;
}

static size_t lora_create_data_packet(uint8_t* buffer, size_t size) {
    /* create data protobuf into buffer, reserve 1 byte at the end for checksum */
    if (buffer == NULL || size == 0) return 0;

    struct obc_lo_ra_frame_t *frame = obc_lo_ra_frame_new(&workspace, sizeof(workspace));
    frame->frame = obc_lo_ra_frame_frame_mcb_frame_e;
    struct obc_mcb_frame_t mcb_frame = {0};
    frame->mcb_frame_p = &mcb_frame;
    create_protobuf_data_frame(frame->mcb_frame_p);

    size_t prefix_size = add_prefix(buffer, size);
    if (prefix_size == 0) return 0; /* not enough room for prefix */

    /* reserve 1 byte for checksum */
    if (size <= prefix_size) return 0;
    size_t max_payload = size - prefix_size;

    size_t data_size = obc_lo_ra_frame_encode(frame, buffer + prefix_size, max_payload);
    if (data_size == 0 || data_size > max_payload) return 0;

    ESP_LOGI(TAG, "Data frame size: %zu", data_size);

    if (prefix_size + data_size > 255) {
        ESP_LOGE(TAG, "Data frame too large to send over LoRa");
        return 0;
    }

    return prefix_size + data_size;
}

static size_t lora_packet(uint8_t* buffer, size_t buffer_size) {
    size_t size = 0;

    size = lora_create_data_packet(buffer, buffer_size);

    ESP_LOGI(TAG, "Sending LoRa frame -> size: %d", size);

    return size;
}

bool initialize_lora(uint32_t frequency_khz, uint32_t transmiting_period) {
    RETURN_ON_FALSE(lora_hw_spi_add_device(VSPI_HOST));
    RETURN_ON_FALSE(lora_hw_set_gpio());
    RETURN_ON_FALSE(lora_hw_attach_d0_interrupt(lora_task_irq_notify));
    lora_struct_t lora = {._spi_transmit = lora_hw_spi_transmit,
                          ._delay = lora_hw_delay,
                          ._gpio_set_level = lora_hw_gpio_set_level,
                          .log = lora_hw_log,
                          .rst_gpio_num = CONFIG_LORA_RS,
                          .cs_gpio_num = CONFIG_LORA_CS,
                          .d0_gpio_num = CONFIG_LORA_D0,
                          .implicit_header = 0,
                          .frequency = 0};
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