// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "state_machine_wrapper.h"
#include "lora_task.h"
#include "lora_hw_config.h"
#include "spi.h"
#include "utils.h"

#define TAG "INIT"

inline static void CHECK_RESULT_ESP(esp_err_t res, char* message) {
    if (res == ESP_OK) {
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(res));
    esp_restart();
}

inline static void CHECK_RESULT_BOOL(esp_err_t res, char *message) {
    if (res == true) {
        return;
    }
    ESP_LOGE(TAG, "Init error %s", message);
    esp_restart();
}


// change to error from error api
static void temp_on_error(ENA_ERROR error) {
    ESP_LOGE(TAG, "ESP NOW ERROR %d", error);
}

static bool init_state_machine(void) {
    state_machine_task_cfg_t task_cfg = {
        .stack_depth = 8000,
        .core_id = APP_CPU_NUM,
        .priority = 3,
    };
    state_config_t *cfg = NULL;
    uint8_t number_of_states;

    ESP_LOGI(TAG, "Initializing state machine");
    SM_Response status = SM_OK;
    SM_init();
    SMW_get_states_config(&cfg, &number_of_states);
    status = SM_set_states(cfg, number_of_states);
    status |= SM_run(&task_cfg);
    return status == SM_OK ? true : false;
}

static bool init_esp_now(void) {
    esp_err_t status = ESP_OK;
    uint8_t mac_address[] = MCB_MAC;
    ENA_config_t cfg = {
      .stack_depth = 8000,
      .priority = 3,
      .core_id = APP_CPU_NUM,
    };
    status |= ENA_init(mac_address);
    status |= ENA_register_device(&esp_now_broadcast);
    status |= ENA_register_device(&esp_now_pitot);
    // status |= ENA_register_device(&esp_now_vent_valve);
    // status |= ENA_register_device(&esp_now_main_valve);
    // status |= ENA_register_device(&esp_now_tanwa);
    status |= ENA_register_error_handler(temp_on_error);
    status |= ENA_run(&cfg);

    return status == ESP_OK ? true : false;
}


static bool init_spi(void) {
    return spi_init(VSPI_HOST, 23, 19, 18);
}

static void lora_process(uint8_t *packet, size_t packet_size) {
    ESP_LOGI(TAG, "Packet %s", packet);
}

static size_t lora_packet(uint8_t *buffer, size_t buffer_size) {
    static int i = 0;
    i += 1;
    return snprintf((char*)buffer, buffer_size, "Hello %d\n", i);
}

static bool init_lora(void) {
    RETURN_ON_FALSE(lora_hw_spi_add_device(VSPI_HOST));
    RETURN_ON_FALSE(lora_hw_set_gpio());
    RETURN_ON_FALSE(lora_hw_attach_d0_interrupt(lora_task_irq_notifi));
    lora_struct_t lora = {
        ._spi_transmit = lora_hw_spi_transmit,
        ._delay = lora_hw_delay,
        ._gpio_set_level = lora_hw_gpio_set_level,
        .log = lora_hw_log,
        .rst_gpio_num = 16,
        .cs_gpio_num = 4,
        .d0_gpio_num = 17,
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


static void TASK_init(void *arg) {
    CHECK_RESULT_BOOL(init_esp_now(), "ESP_NOW");
    CHECK_RESULT_BOOL(init_state_machine(), "STATE_MACHINE");
    CHECK_RESULT_BOOL(init_spi(), "SPI");
    CHECK_RESULT_BOOL(init_lora(), "LORA");
    vTaskDelete(NULL);
}

void run_init_task(void) {
    xTaskCreatePinnedToCore(
        TASK_init,
        "Init",
        8800,
        NULL,
        20,
        NULL,
        PRO_CPU_NUM);
}
