#ifndef UTILS_H
#define UTILS_H

#define RETURN_ON_ERROR(x) if (x != ESP_OK) return x;
#define RETURN_FALSE_ON_ERROR(x) if (x != ESP_OK) return false;
#define RETURN_ON_FALSE(x) if (x == false) return x;

#endif