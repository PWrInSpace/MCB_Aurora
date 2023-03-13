// Copyright 2022 PWrInSpace, Kuba
#include "init_task.h"
#include "esp_log.h"
// #include "esp_now_api.h"

// #define TAG "INIT"

// static void on_rec(uint8_t *data, size_t len) {
//     ESP_LOGI(TAG, "RECEIVED MESSAGE :D");
//     DataToObc x;
//     memcpy(&x, data, len);

//     ESP_LOGI(TAG, "RECEIVED %d %d", x.wakenUp, x.uptime);
// }

// static void on_err(ENA_ERROR err) { ESP_LOGE(TAG, "ERROR :C %d", err); }

// const ENA_device_t test_dev = {
//     .peer = {.peer_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, .channel = 0},
//     .on_receive = on_rec,
// };

// const ENA_device_t test_dev2 = {
//     .peer = {.peer_addr = {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}, .channel = 0},
//     .on_receive = on_rec,
// };


// static esp_err_t init_esp_now_api(void) {
//     ENA_config_t cfg = {
//       .mac_address = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20},
//       .stack_depth = 8000,
//       .priority = 3,
//       .core_id = APP_CPU_NUM,
//     };
//     ENA_init(&cfg);
//     ENA_register_device(&test_dev);
//     ENA_register_device(&test_dev2);
//     ENA_register_error_handler(on_err);
// }

// void TASK_init(void *arg) {
//     init_esp_now_api();
// }
