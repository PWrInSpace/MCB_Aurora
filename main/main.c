// Copyright 2022 PWrInSpace, Kuba
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdcard.h"
#include "spi.h"
#include "esp_timer.h"
#include "i2c.h"
#include "flash.h"
#include "driver/gpio.h"
#include "w25q64.h"
spi_t spi;
i2c_t i2c;
sd_card_t sd;

#define TAG "AURORA"


void app_main(void) {
    while (1) {
        ESP_LOGI(TAG, "Hello world!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
