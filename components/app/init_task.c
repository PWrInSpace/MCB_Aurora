// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "state_machine.h"
#include "esp_now_api.h"

#define TAG "INIT"

inline static void print_log_and_restart(void) {
    ESP_LOGE(TAG, "Path %s", __FILE__);
    ESP_LOGE(TAG, "Line %s", __LINE__);
    esp_restart();
}

inline static void CHECK_RESULT_ESP(esp_err_t res) {
    if (res == ESP_OK) {
        return;
    }

    ESP_LOGE(TAG, "Init error %s", esp_err_to_name(res));
    print_log_and_restart();
}

inline static void CHECK_RESULT_BOOL(esp_err_t res) {
    if (res == true) {
        return;
    }
    ESP_LOGE(TAG, "Init error");
    print_log_and_restart();
}


// change to error from error api
static void temp_on_error(ENA_ERROR error) {
    ESP_LOGE(TAG, "ESP NOW ERROR %d", error);
}

static void init_state_machine(void) {
    state_machine_task_cfg_t task_cfg = {
        .stack_depth = 8000,
        .core_id = APP_CPU_NUM,
        .priority = 3,
    };
    state_config_t *cfg = NULL;
    uint8_t number_of_states;

    ESP_LOGI(TAG, "Initializing state machine");

    SM_init();
    SMW_get_states_config(&cfg, &number_of_states);
    SM_set_states(cfg, number_of_states);
    SM_run(&task_cfg);
}

static esp_err_t init_esp_now(void) {
    ENA_config_t cfg = {
      .mac_address = MCB_MAC,
      .stack_depth = 8000,
      .priority = 3,
      .core_id = APP_CPU_NUM,
    };
    ENA_init(&cfg);
    ENA_register_device(&esp_now_broadcast);
    ENA_register_device(&esp_now_pitot);
    ENA_register_device(&esp_now_vent_valve);
    ENA_register_device(&esp_now_main_valve);
    ENA_register_device(&esp_now_tanwa);
    ENA_register_error_handler(temp_on_error);
}

void TASK_init(void *arg) {
    CHECK_RESULT_ESP(init_esp_now_api());
    // CHECK_RESULT_BOOL(init_state_machine());
    CHECK_RESULT_ESP(ESP_FAIL);
}
