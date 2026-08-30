// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "uart.h"

#define TAG "LORA_T"

static uint8_t buffer[1024];
static StaticMessageBuffer_t message_buffer;

static struct {
    lora_struct_t lora;
    lora_task_process_rx_packet process_packet_fnc;
    lora_task_get_tx_packet get_tx_packet_fnc;
    lora_state_t lora_state;
    uint8_t tx_buffer[512];
    size_t tx_buffer_size;
    MessageBufferHandle_t rx_queue;

    SemaphoreHandle_t irq_notification;
    TaskHandle_t task;
    TaskHandle_t receive_task;
    uint32_t receive_window_period;
} gb;

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

static void lora_change_state_to_receive() {
    if (gb.lora_state == LORA_RECEIVE) {
        return;
    }

    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_write_reg(&gb.lora, REG_IRQ_FLAGS, 0xFF);
    lora_set_receive_mode(&gb.lora);
    gb.lora_state = LORA_RECEIVE;
}

static void lora_change_state_to_transmit() {
    if (gb.lora_state == LORA_TRANSMIT) {
        return;
    }

    gb.lora_state = LORA_TRANSMIT;
}

static size_t on_lora_receive(uint8_t *rx_buffer, size_t buffer_len) {
    size_t len = 0;
    if (lora_received(&gb.lora) == LORA_OK) {
        len = lora_receive_packet(&gb.lora, rx_buffer, buffer_len);
        rx_buffer[len] = '\0';
        xMessageBufferSend(gb.rx_queue, rx_buffer, len, 100);
    }
    return len;
}

static void transmit_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));
    // uart_write_logical(UART_LOGICAL_TELEMETRY, gb.tx_buffer, gb.tx_buffer_size);
    lora_send_packet(&gb.lora, gb.tx_buffer, gb.tx_buffer_size);
}

static void lora_receive_task(void *arg) {
    uint8_t msg[255];

    while (1) {
        size_t msg_size = xMessageBufferReceive(gb.rx_queue, &msg, 255, portMAX_DELAY);
        ESP_LOGI(TAG, "Received %d", msg_size);
        gb.process_packet_fnc(msg, msg_size);
    }
}

void lora_task(void *arg) {
    uint8_t rx_buffer[512];
    size_t rx_packet_size = 0;

    while (1) {
        // 1. Zawsze wymuszamy przejście w tryb nasłuchu i czyścimy flagi
        // (to wykasuje to, co przyszło w trakcie samego nadawania, ale to nieuniknione w
        // Half-Duplex)
        lora_change_state_to_receive();

        TickType_t start_tick = xTaskGetTickCount();
        TickType_t delay_ticks = pdMS_TO_TICKS(gb.receive_window_period);

        // Wyczyść ewentualne zaległe powiadomienia z przerwań RTOS
        ulTaskNotifyTake(pdTRUE, 0);

        // Safety net: ratunek jeśli coś wpadło ułamek sekundy temu
        if (lora_read_reg(&gb.lora, REG_IRQ_FLAGS) & IRQ_RX_DONE_MASK) {
            rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
            if (rx_packet_size > 0) {
                lora_set_receive_mode(&gb.lora);
            } else {
                lora_write_reg(&gb.lora, REG_IRQ_FLAGS, 0xFF);
            }
        }

        // 2. GŁÓWNE OKIENKO 500 ms
        while (1) {
            TickType_t current_tick = xTaskGetTickCount();
            if (current_tick - start_tick >= delay_ticks) {
                break;  // Czas okienka minął, wychodzimy nadawać!
            }
            TickType_t remaining_ticks = delay_ticks - (current_tick - start_tick);

            // Czekamy na przerwanie (RXDONE) przez pozostały czas
            if (ulTaskNotifyTake(pdTRUE, remaining_ticks) == pdTRUE) {
                rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));

                if (rx_packet_size > 0) {
                    lora_set_receive_mode(&gb.lora);
                } else {
                    // Dobra praktyka: zabezpieczenie przed zablokowaniem DIO0 na błędnej ramce
                    lora_write_reg(&gb.lora, REG_IRQ_FLAGS, 0xFF);
                }
            }
        }

        // 3. OKIENKO SIĘ SKOŃCZYŁO - NADAWANIE
        lora_change_state_to_transmit();
        transmit_packet();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool lora_task_init(lora_api_config_t *cfg) {
    if (cfg == NULL) {
        return false;
    }

    if (cfg->process_rx_packet_fnc == NULL || cfg->get_tx_packet_fnc == NULL) {
        return false;
    }

    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    gb.get_tx_packet_fnc = cfg->get_tx_packet_fnc;
    gb.receive_window_period = cfg->transmiting_period;
    gb.rx_queue = xMessageBufferCreateStatic(1024, buffer, &message_buffer);
    memcpy(&gb.lora, cfg->lora, sizeof(lora_struct_t));

    lora_init(&gb.lora);
    lora_set_frequency(&gb.lora, cfg->frequency_khz * 1e3);
    lora_set_bandwidth(&gb.lora, LORA_TASK_BANDWIDTH);
    lora_set_spreading_factor(&gb.lora, LORA_TASK_SPREADING_FACTOR);
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    if (LORA_TASK_CRC_ENABLE) {
        lora_enable_crc(&gb.lora);
    } else {
        lora_disable_crc(&gb.lora);
    }

    gb.irq_notification = xSemaphoreCreateBinary();
    if (gb.irq_notification == NULL) {
        return false;
    }

    ESP_LOGD(TAG, "Starting timer");
    lora_change_state_to_receive();
    // turn_on_receive_window_timer();

    xTaskCreatePinnedToCore(lora_task, "LoRa task", LORA_TASK_STACK_DEPTH, NULL, LORA_TASK_PRIORITY,
                            &gb.task, LORA_TASK_CPU_NUM);

    xTaskCreatePinnedToCore(lora_receive_task, "Receive task", LORA_TASK_STACK_DEPTH, NULL,
                            LORA_TASK_PRIORITY - 1, &gb.receive_task, LORA_TASK_CPU_NUM);

    if (gb.task == NULL) {
        return false;
    }
    {
        UBaseType_t high = uxTaskGetStackHighWaterMark(gb.task);
        ESP_LOGI(TAG, "LoRa task %s stack high water mark: %u", pcTaskGetName(gb.task),
                 (unsigned)high);
    }
    return true;
}

bool lora_change_frequency(uint32_t frequency_khz) {
    if (frequency_khz < 4e5 || frequency_khz > 1e6) {
        return false;
    }

    if (lora_set_frequency(&gb.lora, frequency_khz * 1000) != LORA_OK) {
        return false;
    }

    return true;
}

bool lora_change_receive_window_period(uint32_t period_ms) {
    gb.receive_window_period = period_ms;
    return true;
}
