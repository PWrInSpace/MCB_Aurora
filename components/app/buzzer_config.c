#include "buzzer_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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