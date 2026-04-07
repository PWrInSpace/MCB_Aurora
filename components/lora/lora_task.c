// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "uart.h"

#include <string.h>

#define TAG "LORA_T"

static struct {
    lora_task_validate_rx_packet validate_packet_fnc;
    lora_task_process_rx_packet process_packet_fnc;
    lora_task_get_tx_packet get_tx_packet_fnc;
    uint8_t tx_buffer[512];
    size_t tx_buffer_size;
    uint8_t rx_buffer[512];
    size_t rx_buffer_size;

    TaskHandle_t rx_task;
    TaskHandle_t tx_task;
    TimerHandle_t receive_window_timer;
} gb;

static void receive_packet(void) {
    // turn_of_receive_window_timer();
    if (gb.validate_packet_fnc == NULL) {
        return;
    }

    gb.rx_buffer_size = gb.validate_packet_fnc(gb.rx_buffer, sizeof(gb.rx_buffer));
    if (gb.rx_buffer_size > 0) {
        gb.process_packet_fnc(gb.rx_buffer, gb.rx_buffer_size);
        ESP_LOGD(TAG, "Received %s, len %d", gb.rx_buffer, gb.rx_buffer_size);
    }
}

static void rx_task(void* arg) {
    while (true) {
        receive_packet();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void transmit_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));

    uart_write_logical(UART_LOGICAL_TELEMETRY, gb.tx_buffer, gb.tx_buffer_size);
}

static void tx_task(void *arg) {
    while (true) {
        transmit_packet();

        vTaskDelay(pdMS_TO_TICKS(TRANSMIT_DELAY));
    }
}

bool lora_task_init(lora_api_config_t *cfg) {
    assert(cfg != NULL);
    if (cfg == NULL) {
        return false;
    }

    if (cfg->validate_rx_packet_fnc == NULL || cfg->process_rx_packet_fnc == NULL || cfg->get_tx_packet_fnc == NULL) {
        return false;
    }

    gb.validate_packet_fnc = cfg->validate_rx_packet_fnc;
    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    gb.get_tx_packet_fnc = cfg->get_tx_packet_fnc;
    //memcpy(&gb.lora, cfg->lora, sizeof(lora_struct_t));

    // lora_init(&gb.lora);
    // lora_set_frequency(&gb.lora, cfg->frequency_khz * 1e3);
    // lora_set_bandwidth(&gb.lora, LORA_TASK_BANDWIDTH);
    // lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    // if (LORA_TASK_CRC_ENABLE) {
    //     lora_enable_crc(&gb.lora);
    // } else {
    //     lora_disable_crc(&gb.lora);
    // }

    // gb.receive_window_timer =
    //     xTimerCreate("Transmit timer", pdMS_TO_TICKS(cfg->transmitting_period), pdFALSE, NULL,
    //                  on_receive_window_timer);
    // ESP_LOGD(TAG, "Starting timer");
    // lora_change_state_to_receive();
    // turn_on_receive_window_timer();

    xTaskCreatePinnedToCore(
        rx_task,
        "LoRa task RX",
        LORA_TASK_STACK_DEPTH,
        NULL,
        LORA_TASK_PRIORITY,
        &gb.rx_task,
        LORA_TASK_CPU_NUM
        );

    xTaskCreatePinnedToCore(
        tx_task,
        "LoRa task TX",
        LORA_TASK_STACK_DEPTH,
        NULL,
        LORA_TASK_PRIORITY - 1,
        &gb.tx_task,
        LORA_TASK_CPU_NUM);

    if (gb.rx_task == NULL || gb.tx_task == NULL) {
        return false;
    }
    {
        UBaseType_t high = uxTaskGetStackHighWaterMark(gb.rx_task);
        ESP_LOGI(TAG, "LoRa task %s stack high water mark: %u", pcTaskGetName(gb.rx_task), (unsigned)high);
    }
    {
        UBaseType_t high = uxTaskGetStackHighWaterMark(gb.tx_task);
        ESP_LOGI(TAG, "LoRa task %s stack high water mark: %u", pcTaskGetName(gb.tx_task), (unsigned)high);
    }
    return true;
}

bool lora_change_receive_window_period(uint32_t period_ms) {
    if (xTimerChangePeriod(gb.receive_window_timer, pdMS_TO_TICKS(period_ms), 500) == pdFAIL) {
        return false;
    }

    return true;
}

bool lora_change_frequency(uint32_t frequency_khz) {
    if (frequency_khz < 4e5 || frequency_khz > 1e6) {
        return false;
    }

    memset(&gb.tx_buffer, 0, sizeof(gb.tx_buffer));
    sprintf((char *)gb.tx_buffer, "CMD:FREQ:%d", frequency_khz * 1000);
    // uart_write_logical(UART_LOGICAL_TELEMETRY, gb.tx_buffer, strlen((char *)gb.tx_buffer));

    // on_lora_transmit();
    return true;
}