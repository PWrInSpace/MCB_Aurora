// Copyright 2022 PWrInSpace, Kuba
#include "system_timer_config.h"
#include "sd_task.h"
#include "rocket_data.h"
#include "esp_now_config.h"
#include "state_machine.h"
#include "flash_task.h"

void on_sd_timer(void *arg){
    rocket_data_t test = rocket_data_get();
    // UBaseType_t uxHighWaterMark;
    // /* Inspect our own high water mark on entering the task. */
    // uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // ESP_LOGI(TAG, "DDDD %d %d", uxHighWaterMark * 4, test.mcb.uptime);
    SDT_send_data(&test, sizeof(test));
}

static void on_broadcast_timer(void *arg) {
    uint8_t state = SM_get_current_state();
    ENA_send(&esp_now_broadcast, &state, sizeof(state), 0);
}

static void on_flash_data_timer(void *arg) {
    rocket_data_t data = rocket_data_get();
    FT_send_data(&data);
}

static sys_timer_t timers[] = {
    {TIMER_SD_DATA,             on_sd_timer,            NULL,   NULL},
    {TIMER_ESP_NOW_BROADCAST,   on_broadcast_timer,     NULL,   NULL},
    {TIMER_FLASH_DATA,          on_flash_data_timer,    NULL,   NULL}
    // {TIMER_FLASH_DATA, }
};

bool initialize_timers(void) {
    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}