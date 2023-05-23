#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gps_task.h"

static struct {
    gps_task_process process_fnc;
    TaskHandle_t task_handle;
} gb = {
    .process_fnc = NULL,
    .task_handle = NULL,
};

static void task(void *arg) {
    while (1) {
        if (gb.process_fnc != NULL) {
            gb.process_fnc();
        }
        vTaskDelay(pdMS_TO_TICKS(GPS_TASK_PERIOD));
    }
}


bool gps_create_task(gps_task_cfg_t *cfg) {
    if (cfg->process_fnc == NULL) {
        return false;
    }

    gb.process_fnc = cfg->process_fnc;

    xTaskCreatePinnedToCore(
        task,
        "GPS",
        GPS_TASK_DEPTH,
        NULL,
        GPS_TASK_PRIORITY,
        &gb.task_handle,
        GPS_TASK_CPU);

    if (gb.task_handle == NULL) {
        return false;
    }

    return true;
}