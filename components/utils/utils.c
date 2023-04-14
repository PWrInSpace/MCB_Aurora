#include "utils.h"
#include "esp_timer.h"

uint32_t get_uptime_ms(void) {
    return esp_timer_get_time() / 1000.0;
}