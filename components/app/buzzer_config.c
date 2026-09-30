#include "buzzer_config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BC";

static TaskHandle_t recovery_music_task_handle = NULL;

bool buzzer_play_notes(const note_t *notes, size_t num_notes) {
    for (int i = 0; i < num_notes; i++) {
        if (notes[i].freq == 0) {
            buzzer_turn_off();
        } else {
            buzzer_change_freq(notes[i].freq);
            buzzer_turn_on();
        }
        vTaskDelay(pdMS_TO_TICKS(notes[i].period));
    }
    buzzer_turn_off();
    return true;
}

static void recovery_music_task(void *arg) {
    buzzer_play_notes(dlugosc_dzwieku_samotnosci, sizeof(dlugosc_dzwieku_samotnosci) / sizeof(note_t));
    vTaskDelay(pdMS_TO_TICKS(1000));
}

esp_err_t start_recovery_music(void) {
    if (xTaskCreatePinnedToCore(
        recovery_music_task,
        "recovery_music_task",
        2048,
        NULL,
        2,
        &recovery_music_task_handle,
        1) != pdPASS) {
        ESP_LOGI(TAG, "Failed to create recovery music task");
        return ESP_FAIL;
    }
    return ESP_OK;
}