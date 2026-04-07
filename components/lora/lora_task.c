// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "uart.h"

#include <string.h>

#define TAG "LORA_T"

static struct {
    lora_task_validate_rx_packet validate_packet_fnc;
    lora_task_process_rx_packet process_packet_fnc;
    lora_task_get_tx_packet get_tx_packet_fnc;
    uint8_t lora_state;
    uint8_t tx_buffer[512];
    size_t tx_buffer_size;

    TaskHandle_t task;
    TimerHandle_t receive_window_timer;
} gb;

static bool wait_until_irq(void) {
    return ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE ? true : false;
}

void IRAM_ATTR lora_task_irq_notify(void *arg) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    /* Defensive: ensure gb.task is valid before notifying from ISR. If it's NULL or corrupted,
       calling vTaskNotifyGiveFromISR may dereference invalid pointer and crash. */
    if (gb.task != NULL) {
        vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    }
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void notify_end_of_rx_window(void) { 
    xTaskNotifyGive(gb.task);
    ESP_LOGD(TAG, "END OF WINDOW");
}

static void on_receive_window_timer(TimerHandle_t timer) {
    notify_end_of_rx_window();
}

static void lora_change_state_to_receive() {
    ESP_LOGD(TAG, "Changing state to receive");
    if (gb.lora_state == LORA_RECEIVE) {
        return;
    }

    gb.lora_state = LORA_RECEIVE;
}

static void lora_change_state_to_transmit() {
    ESP_LOGD(TAG, "Changing state to transmit");
    if (gb.lora_state == LORA_TRANSMIT) {
        return;
    }

    gb.lora_state = LORA_TRANSMIT;
}

void turn_on_receive_window_timer(void) {
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        xTimerReset(gb.receive_window_timer, portMAX_DELAY);
        //ESP_LOGE(TAG, "TIMER IS ACTIVE");
        return;
    }
    xTimerStart(gb.receive_window_timer, portMAX_DELAY);
}

void turn_of_receive_window_timer(void) {
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        xTimerStop(gb.receive_window_timer, portMAX_DELAY);
    }
}


static size_t on_lora_receive(uint8_t *rx_buffer, size_t buffer_len) {
    turn_of_receive_window_timer();

    uint8_t data_len = gb.validate_packet_fnc(rx_buffer, buffer_len);
    ESP_LOGD(TAG, "Received %s, len %d", rx_buffer, data_len);
    return data_len;
}

static void transmit_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));

    uart_write_logical(UART_LOGICAL_TELEMETRY, gb.tx_buffer, gb.tx_buffer_size);
}

static void on_lora_transmit() {
    lora_change_state_to_receive();
    turn_of_receive_window_timer();
    turn_on_receive_window_timer();
}

static void lora_task(void *arg) {
    uint8_t rx_buffer[512];
    size_t rx_packet_size = 0;

    while (true) {
        if (wait_until_irq() == true) {
            // on transmit
            if (gb.lora_state == LORA_TRANSMIT) {
                //ESP_LOGI(TAG, "ON transmit");
                on_lora_transmit();
            // on receive
            } else {
                if (gb.validate_packet_fnc != NULL) {
                    rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
                }
                if (rx_packet_size > 0 && gb.process_packet_fnc != NULL) {
                    gb.process_packet_fnc(rx_buffer, rx_packet_size);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                lora_change_state_to_transmit();
                transmit_packet();
                // qucik fix
                turn_on_receive_window_timer();
            }
        }
        // uart_read_logical(UART_LOGICAL_TELEMETRY, rx_buffer, sizeof(rx_buffer), 1000);
        // uart_write_logical(UART_LOGICAL_TELEMETRY, rx_buffer, sizeof(rx_buffer));
        //uart_write_logical(UART_LOGICAL_TELEMETRY, (uint8_t*) "LORA LOOP", 10);
        vTaskDelay(pdMS_TO_TICKS(10));
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

    gb.receive_window_timer =
        xTimerCreate("Transmit timer", pdMS_TO_TICKS(cfg->transmitting_period), pdFALSE, NULL,
                     on_receive_window_timer);
    ESP_LOGD(TAG, "Starting timer");
    lora_change_state_to_receive();
    turn_on_receive_window_timer();

    xTaskCreatePinnedToCore(lora_task, "LoRa task", LORA_TASK_STACK_DEPTH, NULL, LORA_TASK_PRIORITY,
                            &gb.task, LORA_TASK_CPU_NUM);

    if (gb.task == NULL) {
        return false;
    }
    {
        UBaseType_t high = uxTaskGetStackHighWaterMark(gb.task);
        ESP_LOGI(TAG, "LoRa task %s stack high water mark: %u", pcTaskGetName(gb.task), (unsigned)high);
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