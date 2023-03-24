#include "lora_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"

#include "esp_log.h"

#define TAG "LORA_T"

static struct {
    lora_dev_id dev_id;
    lora_state_t lora_state;
    lora_struct_t lora;

    SemaphoreHandle_t irq_notification;
    TaskHandle_t task;
    TimerHandle_t receive_window_timer;
}gb;

static bool check_lora_dev_id(lora_dev_id dev_id) {
    if (dev_id == BORADCAST_DEV_ID) {
        return true;
    }
    // remove privilage mask and check id
    return (dev_id >> 1) == (gb.dev_id >> 1) ? true : false;
}

static bool check_dev_id_privilage_mode(lora_dev_id dev_id) {
    return (dev_id & PRIVILAGE_MASK) > 0 ? true : false;
}

static bool wait_until_irq(void) {
    // return xSemaphoreTake(gb.irq_notification, portMAX_DELAY) == pdTRUE ? true : false;
    return ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE ? true : false;
}

void lora_task_irq_notifi(void) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    // xSemaphoreGiveFromISR(gb.irq_notification, &higher_priority_task_woken);
    vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void notify_end_of_rx_window(void) {
    // xSemaphoreGive(gb.irq_notification);
    xTaskNotifyGive(gb.task);
}

static void init_irq_norification(void) {
    // xSemaphoreTake(gb.irq_notification, portMAX_DELAY);
}

static void lora_change_state(lora_state_t new_state) {
    gb.lora_state = new_state;
}

static void on_lora_receive(uint8_t *rx_buffer, size_t buffer_len) {
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_TXDONE);
    lora_change_state(LORA_TRANSMIT);
    if (lora_received(&gb.lora) == LORA_OK) {
        size_t len = 0;
        len = lora_receive_packet(&gb.lora, rx_buffer, buffer_len);
        rx_buffer[len] = '\0';
        ESP_LOGI(TAG, "Received %s", rx_buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    static int x = 0;
    x += 1;
    char test_packet[200];
    snprintf(test_packet, sizeof(test_packet), "Hello %d\n", x);
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        xTimerStop(gb.receive_window_timer, portMAX_DELAY);
    }
    ESP_LOGW(TAG, "EEEEet");
    lora_send_packet(&gb.lora, (uint8_t *)test_packet, sizeof(test_packet));
}

static void on_lora_transmit() {
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_set_receive_mode(&gb.lora);
    lora_change_state(LORA_RECEIVE);
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        ESP_LOGE(TAG, "TIMER IS ACTIVE");
        return;
    }
    xTimerStart(gb.receive_window_timer, portMAX_DELAY);
}

static void on_receive_window_timer(TimerHandle_t timer) {
    ESP_LOGI(TAG, "Timer notify");
    notify_end_of_rx_window();
}

static void lora_task(void *arg) {
    uint8_t rx_buffer[256];
    size_t rx_len = 0;

    while (1) {
        if (wait_until_irq() == true) {
            if (gb.lora_state == LORA_TRANSMIT) {
                ESP_LOGI(TAG, "On transmit");
                on_lora_transmit();
            } else {
                ESP_LOGI(TAG, "On receive");
                on_lora_receive(rx_buffer, sizeof(rx_buffer));
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

    gb.dev_id = cfg->dev_id;
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
    init_irq_norification();
    if (gb.irq_notification == NULL) {
        ESP_LOGI(TAG, "DUPA");
        return false;
    }

    gb.receive_window_timer = xTimerCreate(
        "Transmit timer",
        LORA_TASK_RECEIVE_WINDOW,
        pdFALSE,
        NULL,
        on_receive_window_timer);
    ESP_LOGI(TAG, "Starting timer");
    xTimerStart(gb.receive_window_timer, portMAX_DELAY);

    xTaskCreatePinnedToCore(
        lora_task,
        "LoRa task",
        LORA_TASK_STACK_DEPTH,
        NULL,
        LORA_TASK_PRIORITY,
        &gb.task,
        LORA_TASK_CPU_NUM);

    if (gb.task == NULL) {
        return false;
    }
    return true;
}




// void lora_api_on_irq(void) {
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     xEventGroupSetBitsFromISR(gb.irq_event, 0, &xHigherPriorityTaskWoken);
//     if( xHigherPriorityTaskWoken )
//     {
//         portYIELD_FROM_ISR();
//     }
// }

// bool lora_wait_for_irq(void) {
// }






