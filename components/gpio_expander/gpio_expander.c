#include "gpio_expander.h"

#include "PCAL6408A.h"
#include "i2c.h"

static struct {
    PCAL6408A_t pca;
} gb;

bool gpio_exp_init(void) {
    gb.pca.dev_address = PCAL6408A_DEV_ADDRESS;
    gb.pca.i2c_read_fnc = i2c_sensors_read;
    gb.pca.i2c_write_fnc = i2c_sensors_write;

    if(PCAL6408A_init(&gb.pca) == false) {
        return false;
    }

    return PCAL6408A_set_mode(&gb.pca, PCAL6408A_OUTPUT);
}

bool gpio_exp_led_set_color(gpioexp_led_colors_t color) {
    // red
    if (PCAL6408A_set_level_pin(&gb.pca, color & 0x01, EXPANDER_LED_PIN_RED) == false) {
        return false;
    }

    // green
    if (PCAL6408A_set_level_pin(&gb.pca, (color >> 1) & 0x01, EXPANDER_LED_PIN_GREEN) == false) {
        return false;
    }

    // blue
    if (PCAL6408A_set_level_pin(&gb.pca, (color >> 2) & 0x01, EXPANDER_LED_PIN_BLUE) == false) {
        return false;
    }

    return true;
}

bool gpio_exp_camera_turn_on(void) {
    return PCAL6408A_set_level_pin(&gb.pca, PCAL6408A_HIGH, EXPANDER_CAMERA_PIN);
}

bool gpio_exp_camera_turn_off(void) {
    return PCAL6408A_set_level_pin(&gb.pca, PCAL6408A_LOW, EXPANDER_CAMERA_PIN);
}

bool gpio_exp_buzzer_init(void) {
    return PCAL6408A_set_level_pin(&gb.pca, PCAL6408A_LOW, EXPANDER_BUZZER_PIN);
}

bool gpio_exp_buzzer_turn_on(void) {
    return PCAL6408A_set_level_pin(&gb.pca, PCAL6408A_HIGH, EXPANDER_BUZZER_PIN);
}

bool gpio_exp_buzzer_turn_off(void) {
    return PCAL6408A_set_level_pin(&gb.pca, PCAL6408A_LOW, EXPANDER_BUZZER_PIN);
}