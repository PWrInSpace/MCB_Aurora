// // Copyright 2022 PWrInSpace

// #include <stdio.h>
// #include "unity.h"
// #include "state_machine.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/timers.h"

// #define TEST_VALUE 10

// REFACTOR NEEDED

// static struct {
//     int launchpad_callback;
//     int flight_callback;
//     int air_brake_callback;
//     int apogee_callback;
//     int end_test;
//     int ground_callback;
// }test_sm = {
//     .launchpad_callback = 0,
//     .flight_callback = 0,
//     .air_brake_callback = 0,
//     .apogee_callback = 0,
//     .end_test = 0,
//     .ground_callback = 0,
// };
// #include "esp_log.h"
// #define TAG "DUPA"
// void launchpad_callback(void *arg) {
//     ESP_LOGI(TAG, "LAUNCHPAD CALLBACK");
//     test_sm.launchpad_callback = TEST_VALUE;
// }

// void flight_callback(void *arg) {
//     ESP_LOGI(TAG, "FLIGHT CALLBACK");
//     test_sm.flight_callback = TEST_VALUE;
// }

// void air_brake_callback(void *arg) {
//     ESP_LOGI(TAG, "AIRBRAKE CALLBACK");
//     test_sm.air_brake_callback = TEST_VALUE;
// }

// void apogee_callback(void *arg) {
//     ESP_LOGI(TAG, "APOGEE CALLBACK");
//     test_sm.apogee_callback = TEST_VALUE;
// }

// void on_ground_callback(void *arg) {
//     ESP_LOGI(TAG, "GROUND CALLBACK");
//     test_sm.ground_callback = TEST_VALUE;
// }

// void end_test(void) {
//     test_sm.end_test = TEST_VALUE;
// }

// typedef enum {
//     LAUNCHPAD,
//     ASCENT,
//     AIRBRAKE,
//     DESCENT,
//     GROUND,
// } states;

// state_config_t states_config[] = {
//     {LAUNCHPAD, launchpad_callback, NULL},
//     {ASCENT, flight_callback, NULL},
//     {AIRBRAKE, air_brake_callback, NULL},
//     {DESCENT, apogee_callback, NULL},
//     {GROUND, on_ground_callback, NULL}
// };



// TEST_CASE("State machine init without all states", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err |= SM_run();

//     TEST_ASSERT_TRUE(err != SM_OK);
//     SM_destroy();
// }

// TEST_CASE("State machine init witch zero states", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err = SM_set_states(states_config, 0);
//     TEST_ASSERT_TRUE(err != SM_OK);\
//     SM_destroy();
// }

// TEST_CASE("State machine init correct", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err = SM_set_states(states_config, sizeof(states_config) / sizeof(state_config_t));
//     TEST_ASSERT_TRUE(err == SM_OK);
//     SM_destroy();
// }

// TEST_CASE("State machine check change to second state", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err |= SM_set_states(states_config, sizeof(states_config) / sizeof(state_config_t));
//     err |= SM_run();
//     vTaskDelay(10 / portTICK_PERIOD_MS);

//     err |= SM_change_state(ASCENT);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     TEST_ASSERT_EQUAL(SM_OK, err);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.flight_callback);
//     TEST_ASSERT_EQUAL(ASCENT, SM_get_current_state());
//     SM_destroy();
// }

// TEST_CASE("State machine change to invalid state", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err |= SM_set_states(states_config, sizeof(states_config) / sizeof(state_config_t));
//     err |= SM_run();
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(GROUND);
//     TEST_ASSERT_EQUAL(SM_STATE_CHANGE_ERROR, err);
//     SM_destroy();
// }


// TEST_CASE("State machine run all states", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err |= SM_set_states(states_config, sizeof(states_config) / sizeof(state_config_t));
//     err |= SM_run();
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(ASCENT);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(AIRBRAKE);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(DESCENT);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(GROUND);
//     vTaskDelay(10 / portTICK_PERIOD_MS);

//     TEST_ASSERT_EQUAL(SM_OK, err);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.launchpad_callback);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.flight_callback);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.air_brake_callback);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.apogee_callback);
//     TEST_ASSERT_EQUAL(SM_OK, err);
//     SM_destroy();
// }

// TEST_CASE("State machine check end loop", "[sm]") {
//     SM_Response err = SM_OK;
//     SM_init();
//     err |= SM_set_states(states_config, sizeof(states_config) / sizeof(state_config_t));
//     err |= SM_set_end_function(end_test, 10);
//     err |= SM_run();
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(ASCENT);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(AIRBRAKE);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(DESCENT);
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     err |= SM_change_state(GROUND);
//     vTaskDelay(50 / portTICK_PERIOD_MS);

//     TEST_ASSERT_EQUAL(SM_OK, err);
//     TEST_ASSERT_EQUAL(TEST_VALUE, test_sm.end_test);
//     SM_destroy();
// }

