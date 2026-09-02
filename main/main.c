// Copyright 2022 PWrInSpace, Kuba

#include "esp_log.h"
#include "init_task.h"

static const char *TAG = "AURORA";

void app_main(void) {
    ESP_LOGI(TAG, "INIT TASK");
    run_init_task();
}
