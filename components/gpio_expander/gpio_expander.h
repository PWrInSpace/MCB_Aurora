#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include <stdbool.h>
#include "sdkconfig.h"

#define EXPANDER_LED_PIN_RED CONFIG_GPIO_EXPANDER_LED_RED
#define EXPANDER_LED_PIN_GREEN CONFIG_GPIO_EXPANDER_LED_GREEN
#define EXPANDER_LED_PIN_BLUE CONFIG_GPIO_EXPANDER_LED_BLUE
#define EXPANDER_CAMERA_THREE_PIN 3
#define EXPANDER_CAMERA_FOUR_PIN 4
#define EXPANDER_GPS_RESET_PIN 5
#define EXPANDER_LORA_RESET_PIN 6

typedef enum {
    WHITE = 0,
    CYAN = 0x01,
    YELLOW = 0x02,
    GREEN = 0x03,
    PURPLE = 0x04,
    BLUE = 0x05,
    RED = 0x06,
    NONE = 0x07
} gpioexp_led_colors_t;

/**
 * @brief Initalize GPIO expander
 * 
 * @return true :D
 * @return false :C
 */
bool gpio_exp_init(void);

/**
 * @brief Set LED color
 *
 * @param color color
 * @return true :D
 * @return false :C
 */
bool gpio_exp_led_set_color(gpioexp_led_colors_t color);

bool gpio_exp_camera_three_turn_on(void);

bool gpio_exp_camera_three_turn_off(void);

bool gpio_exp_camera_four_turn_on(void);

bool gpio_exp_camera_four_turn_off(void);

#endif