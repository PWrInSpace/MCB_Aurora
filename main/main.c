// Copyright 2022 PWrInSpace, Kuba
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_now_api.h"
#include "esp_timer.h"
#include "flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c.h"
#include "sdcard.h"
#include "spi.h"
spi_t spi;
i2c_t i2c;
sd_card_t sd;

#define TAG "AURORA"

typedef struct DataToObc {
    bool wakenUp : 1;
    uint32_t uptime;
    // IMPORTANT! To implementation of each module:

    // IMPORTANT END!
} DataToObc;

static void on_rec(uint8_t *data, size_t len) {
    ESP_LOGI(TAG, "RECEIVED MESSAGE :D");
    DataToObc x;
    memcpy(&x, data, len);

    ESP_LOGI(TAG, "RECEIVED %d %d", x.wakenUp, x.uptime);
}

static void on_err(ENA_ERROR err) { ESP_LOGE(TAG, "ERROR :C %d", err); }

const ENA_device_t test_dev = {
    .peer = {.peer_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, .channel = 0},
    .on_receive = on_rec,
};

const ENA_device_t test_dev2 = {
    .peer = {.peer_addr = {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}, .channel = 0},
    .on_receive = on_rec,
};

void app_main(void) {
  ENA_config_t cfg = {
      .mac_address = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20},
      .stack_depth = 8000,
      .priority = 3,
      .core_id = APP_CPU_NUM,
  };
  ENA_init(&cfg);
  ENA_register_device(&test_dev);
  ENA_register_device(&test_dev2);
  ENA_register_error_handler(on_err);

  while (1) {
    ESP_LOGI(TAG, "Hello world! 1234");
    uint8_t x = 4;
    ENA_send(&test_dev2, &x, sizeof(x), 3);
    ENA_send(&test_dev, &x, sizeof(x), 0);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

// esp_err_t spi_initialize(void) {
//     esp_err_t ret;
//     spi_bus_config_t bus = {
//       .miso_io_num = 19,
//       .mosi_io_num = 23,
//       .sclk_io_num = 18,
//       .quadwp_io_num = -1,
//       .quadhd_io_num = -1,
//       .max_transfer_sz = 4000,
//    };

//    ret = spi_bus_initialize(VSPI_HOST, &bus, SDSPI_DEFAULT_DMA);
//    assert(ret == ESP_OK);
//    return ret;
// }

// esp_err_t spi_add_lora(spi_device_handle_t *spi) {
//     esp_err_t ret;
//     spi_device_interface_config_t dev = {
//        .clock_speed_hz = 9000000,
//        .mode = 0,
//        .spics_io_num = -1,
//        .queue_size = 1,
//        .flags = 0,
//        .pre_cb = NULL
//     };
//     ret = spi_bus_add_device(VSPI_HOST, &dev, spi);
//     assert(ret == ESP_OK);
//     return ret;
// }

// void app_main(void) {
//     spi_device_handle_t spi;
//     sd_card_t sd;
//     spi_initialize();
//     SD_init(&sd, VSPI_HOST, 5, MOUNT_POINT);
//     spi_add_lora(&spi);
//     lora_init(&spi);
//     lora_set_frequency(867000000);
//     lora_disable_crc();
//     lora_set_spreading_factor(7);
//     lora_set_bandwidth(250000);
//     lora_set_tx_power(17);

//     uint8_t test[] = "Hello space\n";
//     char test_sd[] = "Uga buga TABAlugA!\n";
//     while (1) {
//         SD_write(&sd, "/sdcard/ugabuga.txt", test_sd, sizeof(test_sd));
//         lora_send_packet(test, sizeof(test));
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }

// // #include "console.h"
// // #include "esp_log.h"

// // static int dupa(int a, char** b) {
// //     ESP_LOGW("DUPA", "DUPA1234");
// //     return 0;
// // }

// // static esp_console_cmd_t dupsko[] = {
// //     {"Dupa", "test", NULL, dupa, NULL},
// // };

// // void app_main(void) {
// //     console_init();
// //     console_register_commands(dupsko, sizeof(dupsko)/sizeof(dupsko[0]));

// //     while(1) {
// //         vTaskDelay(1000 / portTICK_PERIOD_MS);
// //     }
// // }
