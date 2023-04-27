// Copyright 2022 PWrInSpace, Kuba
#include "errors_config.h"

bool initialize_errors(void) {
    error_type_t types[] = {
        ERROR_TYPE_ESP_NOW,
        ERROR_TYPE_MEMORY,
        ERROR_TYPE_MCB,
        ERROR_TYPE_SENSORS,
        ERROR_TYPE_RECOVERY,
        ERROR_TYPE_LAST_EXCEPTION
    };

    return errors_init(types, sizeof(types));
}