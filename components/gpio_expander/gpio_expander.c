#include "gpio_expander.h"
#include "PCA9574.h"
#include "i2c.h"


static struct {
    PCA9574_t pca;
} gb;

bool gpioexp_init(void) {
    gb.pca.dev_address = 0x20;
    gb.pca.i2c_read_fnc = i2c_sensors_read;
    gb.pca.i2c_write_fnc = i2c_sensors_write;

    if(PCA9574_init(&gb.pca) == false) {
        return false;
    }

    return PCA9574_set_mode(&gb.pca, PCA9574_OUTPUT);
}

bool gpioexp_led_set_color(gpioexp_led_colors_t color) {
    // red
    if (PCA9574_set_level_pin(&gb.pca, (color & 0x01), EXPANDER_LED_PIN_RED) == false) {
        return false;
    }

    // green
    if (PCA9574_set_level_pin(&gb.pca, ((color >> 1) & 0x01), EXPANDER_LED_PIN_GREEN) == false) {
        return false;
    }

    // blue
    if (PCA9574_set_level_pin(&gb.pca, ((color >> 2) & 0x01), EXPANDER_LED_PIN_BLUE) == false) {
        return false;
    }

    return true;
}

bool gpioexp_camera_turn_on(void) {
    return PCA9574_set_level_pin(&gb.pca, PCA9574_HIGH, EXPANDER_CAMERA_PIN);
}

bool gpioexp_camera_turn_off(void) {
    return PCA9574_set_level_pin(&gb.pca, PCA9574_LOW, EXPANDER_CAMERA_PIN);
}