#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define RETURN_ON_ERROR(x) if (x != ESP_OK) return false
#define RETURN_FALSE_ON_ERROR(x) if (x != ESP_OK) return false
#define RETURN_ON_FALSE(x) if (x == false) return false


uint32_t get_uptime_ms(void);

#endif