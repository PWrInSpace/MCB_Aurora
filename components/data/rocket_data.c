#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

static struct {
    rocket_data_t rocket_data;
    SemaphoreHandle_t data_mutex;
} gb;

bool rocket_data_init(void) {
    memset(&gb.rocket_data, 0, sizeof(gb.rocket_data));
    gb.data_mutex = NULL;
    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }

    return true;
}

void rocket_data_update_main_valve(main_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.main_valve, &data, sizeof(gb.rocket_data.main_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_vent_valve(vent_valve_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.vent_valve, &data, sizeof(gb.rocket_data.vent_valve));
    xSemaphoreGive(gb.data_mutex);
}


void rocket_data_update_recovery(recovery_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.recovery, &data, sizeof(gb.rocket_data.recovery));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_mcb(mcb_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.rocket_data.mcb, &data, sizeof(gb.rocket_data.mcb));
    xSemaphoreGive(gb.data_mutex);
}
static rocket_data_t tmp;
rocket_data_t rocket_data_get(void) {
    // xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data;
    // xSemaphoreGive(gb.data_mutex);
    mcb_update_struct(&tmp.mcb);
    return tmp;
}

mcb_data_t rocket_data_get_mcb(void) {
    mcb_data_t tmp;
    mcb_update_struct(&tmp);
    return tmp;
}

main_valve_data_t rocket_data_get_main_valve(void) {
    main_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.main_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

vent_valve_data_t rocket_data_get_vent_valve(void) {
    vent_valve_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.vent_valve;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

recovery_data_t rocket_data_get_recovery(void) {
    recovery_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.rocket_data.recovery;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}