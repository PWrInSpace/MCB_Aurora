#include "lora_task_config.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "utils.h"
#include "lora.pb-c.h"

#define TAG "LORA_C"


/**
 * @brief 
 * 


 *  TIMERY TWORZONE W CONFIGACH >???????


 * 
 * @param packet 
 * @param packet_size 
 */



static void lora_process(uint8_t *packet, size_t packet_size) {
    ESP_LOGI(TAG, "Packet %s", packet);
}

static size_t lora_packet(uint8_t *buffer, size_t buffer_size) {
    static int i = 0;
    i += 1;
    MainValveFrame main_valve_frame;
    main_valve_frame__init(&main_valve_frame);
    main_valve_frame.battery = 21.3;
    main_valve_frame.thermocuple1 = 321;
    main_valve_frame.thermocuple2 = 123;
    VentValveFrame vent_valve_frame;
    vent_valve_frame__init(&vent_valve_frame);
    vent_valve_frame.batteryvoltage = 22.3;
    vent_valve_frame.tankpressure = 62;
    vent_valve_frame.thermistor1 = 12;
    vent_valve_frame.thermistor2 = 22;
    LoRaFrame lora_frame;
    lo_ra_frame__init(&lora_frame);
    lora_frame.mainvalve = &main_valve_frame;
    lora_frame.ventvalve = &vent_valve_frame;
    lora_frame.uptime = 321;
    lora_frame.state = 12;
    size_t size = lo_ra_frame__get_packed_size(&lora_frame);
    lo_ra_frame__pack(&lora_frame, buffer);
    ESP_LOGI(TAG, "FRAME CREATED %lu", size);
    // LoRaFrame * frame = lo_ra_frame__unpack(NULL, size, buffer);
    // ESP_LOGI(TAG, "Received %f", frame->mainvalve->thermocuple1);
    // lo_ra_frame__free_unpacked(frame, NULL);

    return size;
}

bool initialize_lora(void) {
    RETURN_ON_FALSE(lora_hw_spi_add_device(VSPI_HOST));
    RETURN_ON_FALSE(lora_hw_set_gpio());
    RETURN_ON_FALSE(lora_hw_attach_d0_interrupt(lora_task_irq_notify));
    lora_struct_t lora = {
        ._spi_transmit = lora_hw_spi_transmit,
        ._delay = lora_hw_delay,
        ._gpio_set_level = lora_hw_gpio_set_level,
        .log = lora_hw_log,
        .rst_gpio_num = CONFIG_LORA_RS,
        .cs_gpio_num = CONFIG_LORA_CS,
        .d0_gpio_num = CONFIG_LORA_D0,
        .implicit_header = 0,
        .frequency = 0
    };
    lora_api_config_t cfg = {
        .lora = &lora,
        .process_rx_packet_fnc = lora_process,
        .get_tx_packet_fnc = lora_packet,
    };
    RETURN_ON_FALSE(lora_task_init(&cfg));
    return true;
}