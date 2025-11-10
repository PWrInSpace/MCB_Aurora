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
#include "errors_config.h"
#include "gen_pysd.h"
#include "buzzer_pwm.h"
#include "commands_config.h"

void on_sd_timer(void *arg){
    rocket_data_t *p = malloc(sizeof(rocket_data_t));
    if (p == NULL) {
        errors_add(ERROR_TYPE_MEMORY, ERROR_MEMORY_UNKNOWN, 0);
        return;
    }
    rocket_data_copy(p);
    if (SDT_send_data(p, sizeof(*p)) == false) {
        errors_add(ERROR_TYPE_MCB, ERROR_MCB_SD_QUEUE_ADD, 0);
    }
    free(p);
}

static void on_broadcast_timer(void *arg) {
    uint8_t state = SM_get_current_state();
    ENA_send(&esp_now_broadcast, &state, sizeof(state), 0);
}

static void on_flash_data_timer(void *arg) {
    rocket_data_t *p = malloc(sizeof(rocket_data_t));
    if (p == NULL) {
        errors_add(ERROR_TYPE_MEMORY, ERROR_MEMORY_UNKNOWN, 0);
        return;
    }
    rocket_data_copy(p);
    if (FT_send_data(p) == false) {
        errors_add(ERROR_TYPE_MCB, ERROR_MCB_FLASH_QUEUE_ADD, 0);
    }
    free(p);
}

static void on_ignition_timer(void *arg) {
    cmd_message_t mess = cmd_create_message(TANWA_FIRE, 0x00);
    ENA_send(&esp_now_tanwa, mess.raw, sizeof(mess.raw), 3);
}

static void on_liftoff_timer(void *arg) {
    if (SM_change_state(LIFT_OFF) != SM_OK) {
        errors_add(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_STATE_CHANGE, 100);
    }
}

static void on_disconnect_timer(void *arg) {
    SM_force_change_state(ABORT);
}

#define TAG "TIM"

static void buzzer_timer(void *arg) {
    static uint8_t state = 1;
    if (state == 0) {
        buzzer_turn_on();
        state = 1;
    } else {
        buzzer_turn_off();
        state = 0;
    }
}

static void connected_dev(void *arg) {
    esp_now_connected_devices_t dev;
    esp_now_get_connected_dev(&dev);
    rocket_data_update_connected_dev(&dev);
    esp_now_clear_connected_dev();
}

static void debug_data(void *arg) {
    const size_t buf_sz = 2048;
    char *buffer = malloc(buf_sz);
    if (buffer == NULL) {
        errors_add(ERROR_TYPE_MEMORY, ERROR_MEMORY_UNKNOWN, 0);
        return;
    }
    rocket_data_t *p = malloc(sizeof(rocket_data_t));
    if (p == NULL) {
        free(buffer);
        errors_add(ERROR_TYPE_MEMORY, ERROR_MEMORY_UNKNOWN, 0);
        return;
    }
    rocket_data_copy(p);
    pysd_create_sd_frame(buffer, buf_sz, *p, true);
    ESP_LOGD(TAG, "%s", buffer);
    free(p);
    free(buffer);
}

bool initialize_timers(void) {
    sys_timer_t timers[] = {
    {.timer_id = TIMER_SD_DATA,             .timer_callback_fnc = on_sd_timer,              .timer_arg = NULL},
    {.timer_id = TIMER_ESP_NOW_BROADCAST,   .timer_callback_fnc = on_broadcast_timer,       .timer_arg = NULL},
    {.timer_id = TIMER_FLASH_DATA,          .timer_callback_fnc = on_flash_data_timer,      .timer_arg = NULL},
    {.timer_id = TIMER_IGNITION,            .timer_callback_fnc = on_ignition_timer,        .timer_arg = NULL},
    {.timer_id = TIMER_LIFTOFF,             .timer_callback_fnc = on_liftoff_timer,         .timer_arg = NULL},
    {.timer_id = TIMER_DISCONNECT,          .timer_callback_fnc = on_disconnect_timer,      .timer_arg = NULL},
    {.timer_id = TIMER_DEBUG,               .timer_callback_fnc = debug_data,               .timer_arg = NULL},
    {.timer_id = TIMER_BUZZER,              .timer_callback_fnc = buzzer_timer,             .timer_arg = NULL},
    {.timer_id = TIMER_CONNECTED_DEV,       .timer_callback_fnc = connected_dev,            .timer_arg = NULL},
};
    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}