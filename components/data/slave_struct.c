// Copyright 2022 PWrInSpace, Kuba
#include <memory.h>
#include "rocket_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

static struct {
    servo_eth_n2_data_t servo_eth_n2;
    servo_n2o_data_t servo_n2o;
    sol_n2o_n2_data_t sol_n2o_n2;
    sol_eth_data_t sol_eth;
    recovery_data_t recovery;
    payload_data_t payload;
    SemaphoreHandle_t data_mutex;
} gb;


bool rocket_data_init(void) {
    memset(&gb, 0, sizeof(gb));
    gb.data_mutex = NULL;
    gb.data_mutex = xSemaphoreCreateMutex();
    if (gb.data_mutex == NULL) {
        return false;
    }

    return true;
}



void rocket_data_update_servo_eth_n2(servo_eth_n2_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.servo_eth_n2, data, sizeof(gb.servo_eth_n2));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_servo_n2o(servo_n2o_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.servo_n2o, data, sizeof(gb.servo_n2o));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_sol_n2o_n2(sol_n2o_n2_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.sol_n2o_n2, data, sizeof(gb.sol_n2o_n2));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_sol_eth(sol_eth_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.sol_eth, data, sizeof(gb.sol_eth));
    xSemaphoreGive(gb.data_mutex);
}

void rocket_data_update_recovery(recovery_data_t *data) {
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    memcpy(&gb.recovery, data, sizeof(gb.recovery));
    xSemaphoreGive(gb.data_mutex);
}

servo_eth_n2_data_t rocket_data_get_servo_eth_n2(void) {
    servo_eth_n2_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.servo_eth_n2;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

servo_n2o_data_t rocket_data_get_servo_n2o(void) {
    servo_n2o_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.servo_n2o;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

sol_n2o_n2_data_t rocket_data_get_sol_n2o_n2(void) {
    sol_n2o_n2_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.sol_n2o_n2;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

sol_eth_data_t rocket_data_get_sol_eth(void) {
    sol_eth_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.sol_eth;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}

recovery_data_t rocket_data_get_recovery(void) {
    recovery_data_t tmp;
    xSemaphoreTake(gb.data_mutex, portMAX_DELAY);
    tmp = gb.recovery;
    xSemaphoreGive(gb.data_mutex);
    return tmp;
}
