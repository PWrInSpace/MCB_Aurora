// Copyright 2022 PWrInSpace, Kuba
#include "system_timer_config.h"
#include "sd_task.h"
#include "rocket_data.h"
#include "esp_now_config.h"
#include "commands.h"
#include "state_machine.h"
#include "state_machine_config.h"
#include "flash_task.h"
#include "esp_log.h"

void on_sd_timer(void *arg){
    rocket_data_t test = rocket_data_get();
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

static void on_ignition_timer(void *arg) {
    cmd_message_t mess = cmd_create_message(0x01, 0x01);
    ENA_send(&esp_now_tanwa, mess.raw, sizeof(mess.raw), 3);    
}

static void on_liftoff_timer(void *arg) {
    SM_change_state(FLIGHT);
}

static void on_disconnect_timer(void *arg) {
    SM_force_change_state(ABORT);
}

bool initialize_timers(void) {
    sys_timer_t timers[] = {
    {.timer_id = TIMER_SD_DATA,          .timer_callback_fnc = on_sd_timer,         .timer_arg = NULL},
    {.timer_id = TIMER_ESP_NOW_BROADCAST,.timer_callback_fnc = on_broadcast_timer,  .timer_arg = NULL},
    {.timer_id = TIMER_FLASH_DATA,       .timer_callback_fnc = on_flash_data_timer, .timer_arg = NULL},
    {.timer_id = TIMER_IGNITION,            .timer_callback_fnc = on_ignition_timer,      .timer_arg = NULL},
    {.timer_id = TIMER_LIFTOFF,             .timer_callback_fnc = on_liftoff_timer,       .timer_arg = NULL},
    {.timer_id = TIMER_DISCONNECT,          .timer_callback_fnc = on_disconnect_timer,    .timer_arg = NULL},
};
    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}