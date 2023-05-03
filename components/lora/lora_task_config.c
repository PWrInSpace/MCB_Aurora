#include "lora_task_config.h"

#include "commands_config.h"
#include "data_to_protobuf.h"
#include "esp_log.h"
#include "lora.pb-c.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "sdkconfig.h"
#include "utils.h"
#include "errors_config.h"
#include "system_timer_config.h"

#define TAG "LORA_C"

static void lora_process(uint8_t *packet, size_t packet_size) {
    LoRaCommand *received = lo_ra_command__unpack(NULL, packet_size, packet);
    if (received != NULL) {
        ESP_LOGI(TAG, "Received LORA_ID %d, DEV_ID %d, COMMAND %d, PLD %d", received->lora_dev_id,
                 received->sys_dev_id, received->command, received->payload);

        cmd_message_t received_command = cmd_create_message(received->command, received->payload);
        lo_ra_command__free_unpacked(received, NULL);
        if (lora_cmd_process_command(received->lora_dev_id, received->sys_dev_id,
                                     &received_command) == false) {
            errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_COMMAND_NOT_FOUND, 200);
            ESP_LOGE(TAG, "Unable to prcess command :C");
            return;
        }

        if (sys_timer_restart(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS) == false) {
            ESP_LOGE(TAG, "Unable to restart timer");
        }
    } else {
        errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_LORA_DECODE, 200);
        ESP_LOGE(TAG, "Unable to decode received package");
    }
}

static size_t lora_packet(uint8_t *buffer, size_t buffer_size) {
    LoRaFrame frame;
    size_t size;
    create_porotobuf_frame(&frame);
    size = lo_ra_frame__pack(&frame, buffer);
    ESP_LOGI(TAG, "Sending LoRa frame -> size: %d", size);

    return size;
}

bool initialize_lora(void) {
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
    };
    RETURN_ON_FALSE(lora_task_init(&cfg));
    return true;
}