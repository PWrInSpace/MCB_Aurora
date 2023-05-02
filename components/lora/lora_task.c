// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define TAG "LORA_T"

static struct {
    lora_struct_t lora;
    lora_task_process_rx_packet process_packet_fnc;
    lora_task_get_tx_packet get_tx_packet_fnc;
    lora_state_t lora_state;
    uint8_t tx_buffer[256];
    size_t tx_buffer_size;

    SemaphoreHandle_t irq_notification;
    TaskHandle_t task;
    TimerHandle_t receive_window_timer;
} gb;

static bool wait_until_irq(void) {
    return ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE ? true : false;
}

void IRAM_ATTR lora_task_irq_notify(void *arg) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void notify_end_of_rx_window(void) { xTaskNotifyGive(gb.task); }

static void on_receive_window_timer(TimerHandle_t timer) { notify_end_of_rx_window(); }

static void lora_change_state_to_receive() {
    // ESP_LOGI(TAG, "Changing state to receive");
    if (gb.lora_state == LORA_RECEIVE) {
        return;
    }

    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_set_receive_mode(&gb.lora);
    gb.lora_state = LORA_RECEIVE;
}

static void lora_change_state_to_transmit() {
    // ESP_LOGI(TAG, "Changing state to transmit");
    if (gb.lora_state == LORA_TRANSMIT) {
        return;
    }

    gb.lora_state = LORA_TRANSMIT;
}

void turn_on_receive_window_timer(void) {
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        ESP_LOGE(TAG, "TIMER IS ACTIVE");
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
    size_t len = 0;
    turn_of_receive_window_timer();
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_TXDONE);
    if (lora_received(&gb.lora) == LORA_OK) {
        len = lora_receive_packet(&gb.lora, rx_buffer, buffer_len);
        rx_buffer[len] = '\0';
        ESP_LOGI(TAG, "Received %s, len %d", rx_buffer, len);
    }
    return len;
}

static void transmint_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));
    lora_send_packet(&gb.lora, gb.tx_buffer, gb.tx_buffer_size);
}

static void on_lora_transmit() {
    lora_change_state_to_receive();
    turn_on_receive_window_timer();
}

static void lora_task(void *arg) {
    uint8_t rx_buffer[256];
    size_t rx_packet_size = 0;

    while (1) {
        if (wait_until_irq() == true) {
            if (gb.lora_state == LORA_TRANSMIT) {
                on_lora_transmit();
            } else {
                rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
                if (rx_packet_size > 0 && gb.process_packet_fnc != NULL) {
                    gb.process_packet_fnc(rx_buffer, rx_packet_size);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                lora_change_state_to_transmit();
                transmint_packet();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool lora_task_init(lora_api_config_t *cfg) {
    assert(cfg != NULL);
    if (cfg == NULL) {
        return false;
    }

    if (cfg->process_rx_packet_fnc == NULL || cfg->get_tx_packet_fnc == NULL) {
        return false;
    }

    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    gb.get_tx_packet_fnc = cfg->get_tx_packet_fnc;
    memcpy(&gb.lora, cfg->lora, sizeof(lora_struct_t));

    lora_init(&gb.lora);
    lora_set_frequency(&gb.lora, LORA_TASK_FREQUENCY_KHZ * 1e3);
    lora_set_bandwidth(&gb.lora, LORA_TASK_BANDWIDTH);
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    if (LORA_TASK_CRC_ENABLE) {
        lora_enable_crc(&gb.lora);
    } else {
        lora_disable_crc(&gb.lora);
    }

    gb.irq_notification = xSemaphoreCreateBinary();
    // init_irq_norification();
    if (gb.irq_notification == NULL) {
        return false;
    }

    gb.receive_window_timer =
        xTimerCreate("Transmit timer", pdMS_TO_TICKS(LORA_TASK_RECEIVE_WINDOW), pdFALSE, NULL,
                     on_receive_window_timer);
    ESP_LOGI(TAG, "Starting timer");
    lora_change_state_to_receive();
    turn_on_receive_window_timer();

    xTaskCreatePinnedToCore(lora_task, "LoRa task", LORA_TASK_STACK_DEPTH, NULL, LORA_TASK_PRIORITY,
                            &gb.task, LORA_TASK_CPU_NUM);

    if (gb.task == NULL) {
        return false;
    }
    return true;
}

bool lora_change_receive_window_period(uint32_t period_ms) {
    if (xTimerChangePeriod(gb.receive_window_timer, pdMS_TO_TICKS(period_ms), 500) == pdFAIL) {
        return false;
    }

    return true;
}
