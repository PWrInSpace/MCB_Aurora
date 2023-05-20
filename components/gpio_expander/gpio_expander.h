#ifndef GPIO_EXPANDER_H
#define GPIO_EXPANDER_H

#include <stdbool.h>
#include "sdkconfig.h"

#define EXPANDER_LED_PIN_RED CONFIG_GPIO_EXPANDER_LED_RED
#define EXPANDER_LED_PIN_GREEN CONFIG_GPIO_EXPANDER_LED_GREEN
#define EXPANDER_LED_PIN_BLUE CONFIG_GPIO_EXPANDER_LED_BLUE
#define EXPANDER_CAMERA_PIN CONFIG_GPIO_EXPANDER_LED_CAMERA 

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

bool gpioexp_init(void);

bool gpioexp_led_set_color(gpioexp_led_colors_t color);

bool gpioexp_camera_turn_on(void);

bool gpioexp_camera_turn_off(void);

#endif