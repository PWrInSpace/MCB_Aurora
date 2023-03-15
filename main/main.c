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
#include "console.h"
#include "flash_task.h"
#include "sd_task.h"

#define TAG "AURORA"


// typedef struct {
//     float x;
//     uint32_t time_stamp;
//     float t[60];
// } data;

// static int read_flash(int argc, char** argv) {
//     esp_log_level_set("*", ESP_LOG_NONE);
//     FILE *file = NULL;
//     file = fopen(FLASH_PATH, "r");
//     if (file == NULL) {
//         CONSOLE_WRITE_E("Unable to open file");
//         return -1;
//     }

//     data d;
//     while (fread(&d, sizeof(d), 1, file)) {
//         CONSOLE_WRITE("%d %f", d.time_stamp, d.x);
//     }
//     fclose(file);

//     CONSOLE_WRITE_G("Read end");
//     esp_log_level_set("*", ESP_LOG_INFO);
//     return 0;
//     return 0;
// }

// static int flash_start(int argc, char** arg ) {
//     // flag = true;
//     FT_erase_and_run_loop();
//     return 0;
// }

// static int flash_terminate(int argc, char ** arg) {
//     FT_terminate();
//     return 0;
// }


// static esp_console_cmd_t cmd[] = {
//     {"flash-read", "123", NULL, read_flash, NULL},
//     {"flash-start", "3223", NULL, flash_start, NULL},
//     {"flash-terminate", "12313", NULL, flash_terminate, NULL}
// };
// static void init_console() {
//     console_init();
//     esp_err_t ret = console_register_commands(cmd, sizeof(cmd)/sizeof(cmd[0]));
//     ESP_LOGW(TAG, "%s", esp_err_to_name(ret));
// }

// static bool can_write() {
//     return true;
// }


// static void flash_task() {
//     flash_task_cfg_t cfg = {
//         .data_size = sizeof(data),
//         .queue_size = 40,
//         .stack_depth = 8000,
//         .core_id = APP_CPU_NUM,
//         .priority = 4,
//         .error_handler_fnc = NULL,
//         .can_write_to_flash_fnc = can_write,
//     };
//     FT_init(&cfg);
// }

esp_err_t spi_initialize(void) {
    esp_err_t ret;
    spi_bus_config_t bus = {
      .miso_io_num = 19,
      .mosi_io_num = 23,
      .sclk_io_num = 18,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(VSPI_HOST, &bus, SDSPI_DEFAULT_DMA);
    assert(ret == ESP_OK);
    return ret;
}


void app_main(void) {
    // init_console();
    // data d = {
    //     .time_stamp = 0,
    //     .x = 12,
    // };
    // flash_task();
    spi_initialize();
    sd_task_cfg_t cfg = {
        .cs_pin = 5,
        .data_path = "data",
        .data_path_size = 9,
        .spi_host = VSPI_HOST,
        .log_path = "log",
        .log_path_size = 5,
        .error_handler_fnc = NULL,
    };

    SDT_init(&cfg);
    char dupa[256];
    int i = 0;
    while (1) {
        snprintf(dupa, sizeof(dupa), "Hello world\n");
        // ESP_LOGI(TAG, "Writing to sd");
        SDT_send_data(dupa, sizeof(dupa));
        snprintf(dupa, sizeof(dupa), "Hello XDDDD\n");
        SDT_send_log(dupa, sizeof(dupa));
        vTaskDelay(pdMS_TO_TICKS(1000));
        i++;
        if (i == 20) {
            ESP_LOGI(TAG, "changing path");
            char path[] = "test123";
            SDT_change_data_path(path, sizeof(path));
        }
        if (i == 40) {
            ESP_LOGI(TAG, "TERMINATING");
            SDT_terminate();
        }
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
//     sd_card_config_t cfg = {
//         .cs_pin = 5,
//         .spi_host = VSPI_HOST,
//         .mount_point = SDCARD_MOUNT_POINT
//     };
//     SD_init(&sd, &cfg);
//     // spi_add_lora(&spi);
//     // lora_init(&spi);
//     // lora_set_frequency(867000000);
//     // lora_disable_crc();
//     // lora_set_spreading_factor(7);
//     // lora_set_bandwidth(250000);
//     // lora_set_tx_power(17);

//     uint8_t test[] = "Hello space\n";
//     char test_sd[] = "Uga buga TABAlugA!\n";
//     while (1) {
//         SD_write(&sd, "/sdcard/ugabuga.txt", test_sd, sizeof(test_sd));
//         // lora_send_packet(test, sizeof(test));
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
