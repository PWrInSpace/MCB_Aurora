#include "buzzer_pwm.h"
#include "driver/ledc.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (12) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 5 kHz

bool buzzer_init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };

    if (ledc_timer_config(&ledc_timer) != ESP_OK) {
        return false;
    }

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    if (ledc_channel_config(&ledc_channel) != ESP_OK) {
        return false;
    }

    return true;
}

bool buzzer_turn_on(void) {
    if (ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY) != ESP_OK) {
        return false;
    }

    // Update duty to apply the new value
    if (ledc_update_duty(LEDC_MODE, LEDC_CHANNEL) != ESP_OK) {
        return false;
    }

    return true;
}

bool buzzer_turn_off(void) {
    if (ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0) != ESP_OK) {
        return false;
    }

    // Update duty to apply the new value
    if (ledc_update_duty(LEDC_MODE, LEDC_CHANNEL) != ESP_OK) {
        return false;
    }

    return true;
}
