#include "state_machine_wrapper.h"
#include <stddef.h>
#include "esp_log.h"

#define TAG "SMW"
static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    ESP_LOGI(TAG, "ON IDLE");

}

static void on_recovery_arm(void *arg) {
    ESP_LOGI(TAG, "ON ARM");

}

static void on_fueling(void *arg) {
    ESP_LOGI(TAG, "ON FUELING");

}

static void on_armed_to_launch(void *arg) {
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");

}

static void on_ready_to_lauch(void *arg) {
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");

}

static void on_countdown(void *arg) {
    ESP_LOGI(TAG, "ON COUNTDOWN");

}

static void on_flight(void *arg) {
    ESP_LOGI(TAG, "ON FLIGHT");

}

static void on_first_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON FIRST_STAGE_RECOV");

}

static void on_second_stage_recovery(void *arg) {
    ESP_LOGI(TAG, "ON SECOND_STAGE_RECOV");

}

static void on_ground(void *arg) {
    ESP_LOGI(TAG, "ON GROUND");

}

static void on_hold(void *arg) {
    ESP_LOGI(TAG, "ON INIT");

}

static void on_abort(void *arg) {
    ESP_LOGI(TAG, "ON INIT");

}

static state_config_t states_cfg[] = {
    {INIT, on_init, NULL},
    {IDLE, on_idle, NULL},
    {RECOVERY_ARM, on_recovery_arm, NULL},
    {FUELING, on_fueling, NULL},
    {ARMED_TO_LAUNCH, on_armed_to_launch, NULL},
    {RDY_TO_LAUNCH, on_ready_to_lauch, NULL},
    {COUNTDOWN, on_countdown, NULL},
    {FLIGHT, on_flight, NULL},
    {FIRST_STAGE_RECOVERY, on_first_stage_recovery, NULL},
    {SECOND_STAGE_RECOVERY, on_second_stage_recovery, NULL},
    {ON_GROUND, on_ground, NULL},
    {HOLD, on_hold, NULL},
    {ABORT, on_abort, NULL},
};


void SMW_get_states_config(state_config_t **cfg, uint8_t *nb_of_states) {
    *cfg = states_cfg;
    *nb_of_states = sizeof(states_cfg)/sizeof(state_config_t);
}