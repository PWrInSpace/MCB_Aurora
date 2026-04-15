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
    QueueHandle_t uart_queue;
} gb;

static void receive_packet(size_t size_to_read) {
    int read_len = uart_read_bytes(UART_NUM_1, gb.rx_buffer, size_to_read, pdMS_TO_TICKS(100));

    if (read_len <= 0) {
        return;
    }
    gb.rx_buffer_size = gb.validate_packet_fnc(gb.rx_buffer, read_len);

    if (gb.rx_buffer_size > 5) {
        gb.process_packet_fnc(gb.rx_buffer, gb.rx_buffer_size);
        ESP_LOGI(TAG, "Valid packet received, len %d", gb.rx_buffer_size);
    }
}

static void rx_task(void* arg) {
    uart_event_t event;
    while (true) {
        if (xQueueReceive(gb.uart_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    receive_packet(event.size);
                    break;

                case UART_FIFO_OVF:
                    ESP_LOGE(TAG, "UART FIFO Overflow!");
                    xQueueReset(gb.uart_queue);
                    break;

                default:
                    // Inne zdarzenia (error, break itp.) ignorujemy
                    break;
            }
        }
    }
}

QueueHandle_t* lora_task_get_uart_queue_ptr(void) {
    return &gb.uart_queue;
}

static void transmit_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        ESP_LOGE(TAG, "get_tx_packet_fnc is NULL");
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));
    ESP_LOG_BUFFER_HEX(TAG, gb.tx_buffer, 16); // Wyświetli pierwsze 16 bajtów w HEX
    uart_write_logical(UART_LOGICAL_TELEMETRY, gb.tx_buffer, gb.tx_buffer_size);
    return;
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
        LORA_TASK_PRIORITY,
        &gb.tx_task,
        LORA_TASK_CPU_NUM);

        // if (gb.rx_task == NULL || gb.tx_task == NULL) {
        //     return false;
        // }
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