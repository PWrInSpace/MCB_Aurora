#ifndef VBAT_WRAPPER
#define VBAT_WRAPPER

#include <stdbool.h>

bool vbat_init(void);

float vbat_read(void);

#endif