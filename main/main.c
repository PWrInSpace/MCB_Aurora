// Copyright 2022 PWrInSpace, Kuba
#include <stdio.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_now_api.h"
#include "esp_timer.h"
#include "flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c.h"
#include "sdcard.h"
#include "spi.h"
#include "driver/gpio.h"
#include "w25q64.h"
#include "console.h"
#include "flash_task.h"
#include "sd_task.h"
#include "init_task.h"
#include "lora_hw_config.h"
#include "lora_task.h"
#include "gen_pysd.h"
#include "uart.h"
#include "ublox_m8.h"
#include "gpio_expander.h"
#include "system_timer_config.h"
#include "console_config.h"
#include "gps_task_config.h"
#include "bmp5_wrapper.h"
#include "bmi08_wrapper.h"
#include "mahony.h"
#include "mag_wrapper.h"
#include "magdwick.h"


#include "sdkconfig.h"
#define TAG "AURORA"


void app_main(void) {
    ESP_LOGI(TAG, "INIT TASK");
    run_init_task();
    vTaskDelete(NULL);
}


// void app_main(void) {
//     spi_init(VSPI_HOST, CONFIG_SPI_MOSI, CONFIG_SPI_MISO, CONFIG_SPI_SCK);
//     esp_flash_t *flash;
//     const esp_flash_spi_device_config_t device_config = {
//         .host_id = VSPI_HOST,
//         .cs_id = 0,
//         .cs_io_num = 4,
//         .io_mode = SPI_FLASH_FASTRD,
//         .freq_mhz = 10,
//     };

//     esp_err_t err = spi_bus_add_flash_device(&flash, &device_config);
//         if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to add", esp_err_to_name(err), err);
//     }

//    err = esp_flash_init(flash);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to initialize external Flash: %s (0x%x)", esp_err_to_name(err), err);
//     }

//     ESP_LOGI(TAG, "INIT TASK");
// }


// void app_main(void) {
//     ESP_LOGI(TAG, "INIT TASK");
//     // run_init_task();

//     i2c_sensors_init();
//     bool res = bmp5_wrapper_init();
//     ESP_LOGE(TAG, "Result %d", res);

//     struct bmp5_sensor_data data;
//     while(1) {
//         if (bmp5_wrapper_get_data(&data) == true) {
//             ESP_LOGI(TAG, "Pressure %.2f \t Temperature %.2f", data.pressure / 100, data.temperature);
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
//     vTaskDelete(NULL);
// }

// void app_main(void) {
//     ESP_LOGI(TAG, "MAG start :D");
//     i2c_sensors_init();

//     bool res = bmp5_wrapper_init();
//     ESP_LOGI(TAG, "Result %d", res);
//     if (bmi08_wrapper_init() == false) {
//         ESP_LOGW(TAG, "DUPA");
//     }

//     ESP_LOGI(TAG, "BMI08 initialzied :D");
//     if (mag_init() == false) {
//         ESP_LOGE(TAG, "INIT ERROR :C");
//     }

//     if (mag_set_continous_mode(FREQ_100HZ, PRD_500) == false) {
//         ESP_LOGE(TAG, "CM ERROR :C");
//     }

//     ESP_LOGI(TAG, ":D");
//     struct bmi08_sensor_data_f acc;
//     struct bmi08_sensor_data_f gyro;
//     struct bmi08_sensor_data_f acc_prev = {0};
//     struct bmi08_sensor_data_f gyro_prev = {0};
//     mmc5983_mag_t mag;
//     mmc5983_mag_t mag_prev = {0};
//     // mahony_t mahony;
//     // mahony_init(10, 0.0, &mahony);
//     double temp[3] = {0};

//     struct mgos_imu_madgwick test; 
//     mgos_imu_madgwick_create(&test);
//     mgos_imu_madgwick_set_params(&test, 100, 0.01);
//     mgos_imu_madgwick_reset(&test);

//     while (true) {
//         if (bmi08_get_acc_data(&acc) == true && bmi08_get_gyro_data(&gyro) == true && mag_data_ready() == true) {
//             // mahony_updateIMU(, );
//             mag_get_data(&mag);
//             // Mahony_update();

//             mgos_imu_madgwick_update(&test, gyro.x, gyro.y, gyro.z, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z);

//             // acc.x = acc.x * 0.8 + acc_prev.x * 0.2;
//             // acc.y = acc.y * 0.8 + acc_prev.y * 0.2;
//             // acc.z = acc.z * 0.8 + acc_prev.z * 0.2;

//             // gyro.x = gyro.x * 0.8 + gyro_prev.x * 0.2;
//             // gyro.y = gyro.y * 0.8 + gyro_prev.y * 0.2;
//             // gyro.z = gyro.z * 0.8 + gyro_prev.z * 0.2;

//             // mag.x = mag.x * 0.8 + mag_prev.x * 0.2;
//             // mag.y = mag.y * 0.8 + mag_prev.y * 0.2;
//             // mag.z = mag.z * 0.8 + mag_prev.z * 0.2;

//             // acc_prev = acc;
//             // gyro_prev = gyro;
//             // mag_prev = mag;

//             // mahony_update(gyro.x, gyro.y, gyro.z, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z, &mahony);
//             // fusion_update(gyro.x, gyro.y, gyro.z, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z);
//             // quaternion_to_euler_ZYX(&mahony.q, &test);
//             // float heading = 360.0 + (atan2(data.y, data.x) * 180 / M_PI);
//             // ESP_LOGI(TAG, "MAG x: %f\ty: %f\tz: %f\t heading %f", data.x, data.y, data.z, heading);
//             // ESP_LOGI(TAG, "ACC x: %f\ty: %f\tz: %f\t GYRO x: %f\ty: %f\tz: %f", acc.x, acc.y, acc.z, gyro.x, gyro.y, gyro.z);

//             // ESP_LOGI(TAG, "MAHONY q0: %f\tq1: %f\tq2: %f\tq3: %f\t", mahony.q.q0, mahony.q.q1, mahony.q.q2, mahony.q.q3);
//             // ESP_LOGI(TAG, "Euelr roll: %f\tpitch: %f\tyaw: %f", &test[0], &test[1], &test[2]);
//             // temp[0] += &test[0];
//             // temp[1] += &test[1];
//             // temp[2] += &test[2];
//             // Mahony_computeAngles();
//             // float roll = getRoll();
//             // float pitch = getPitch();
//             // float yaw = getYaw();

//             // quaternion_t q;
//             // mgos_imu_madgwick_get_quaternion(&test, &q.q0, &q.q1, &q.q2, &q.q3);
//             // printf("w%fwa%fab%fbc%fc\n", q.q0, q.q1, q.q2, q.q3);
//             float yaw, pitch, roll;
//             mgos_imu_madgwick_get_angles(&test, &roll, &pitch, &yaw);
//             printf("y%fyp%fpr%fr\n", yaw, pitch, roll);
//         }

//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }


// void app_main(void) {
//     ESP_LOGI(TAG, "UaRT init");
//     uart_init(CONFIG_UART_PORT_NUM, CONFIG_UART_TX, CONFIG_UART_RX, CONFIG_UART_BAUDRATE);
//     i2c_sensors_init();

//     ESP_LOGI(TAG, "GPS init");
//     // ublox_m8_t ubx = {
//     //     .uart_read_fnc = uart_ublox_read,
//     //     .uart_write_fnc = uart_ublox_write,
//     //     .delay_fnc = test,
//     // };

//     // ublox_m8_init(&ubx);
//     // vTaskDelay(pdMS_TO_TICKS(1000));
//     if (initialize_gps() == false) {
//         ESP_LOGE(TAG, "GPS init error");
//     }
//     ESP_LOGI(TAG, "Expander init");
//     gpioexp_init();
//     gpioexp_led_set_color(CYAN);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(YELLOW);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(PURPLE);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(GREEN);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(RED);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(BLUE);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(WHITE);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpioexp_led_set_color(NONE);

//     initialize_timers();
//     sys_timer_start(TIMER_DISCONNECT, DISCONNECT_TIMER_PERIOD_MS, TIMER_TYPE_ONE_SHOT);
//     init_console();


//     ublox_m8_pvt_t pvt;
//     gps_positioning_t position;
//     while (1) {
//         // ublox_m8_get_ESFALG(&ubx, &esfalg);
//         // if (ublox_m8_get_PVT(&ubx, &pvt) == true) {
//         //     ESP_LOGI(TAG, "Fix type %d", pvt.fix_type);
//         //     ESP_LOGI(TAG, "Sats %d", pvt.numSV);
//         //     ESP_LOGI(TAG, "Lat %f", pvt.lat.data / 10e6);
//         //     ESP_LOGI(TAG, "Long %f", pvt.lon.data / 10e6);
//         //     ESP_LOGI(TAG, "Height %f", pvt.height.data / 10e3);
//         // }
//         position = gps_get_positioning();
//         ESP_LOGI(TAG, "Fix type %d", position.fix_type);
//         ESP_LOGI(TAG, "Sats %d", position.sats_in_view);
//         ESP_LOGI(TAG, "Lat %f", position.latitude);
//         ESP_LOGI(TAG, "Long %f", position.longitude);
//         ESP_LOGI(TAG, "Height %f", position.altitude);
//         uint64_t exp;
//         sys_timer_get_expiry_time(TIMER_DISCONNECT, &exp);
//         ESP_LOGI(TAG, "Timer -> %" PRIu64, (exp - esp_timer_get_time()) / 1000 / 1000);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

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

// esp_err_t spi_initialize(void) {
//     esp_err_t ret;
//     spi_bus_config_t bus = {
//       .miso_io_num = 19,
//       .mosi_io_num = 23,
//       .sclk_io_num = 18,
//       .quadwp_io_num = -1,
//       .quadhd_io_num = -1,
//       .max_transfer_sz = 4000,
//     };

//     ret = spi_bus_initialize(VSPI_HOST, &bus, SDSPI_DEFAULT_DMA);
//     assert(ret == ESP_OK);
//     return ret;
// }


// void app_main(void) {
//     // init_console();
//     // data d = {
//     //     .time_stamp = 0,
//     //     .x = 12,
//     // };
//     // flash_task();
//     spi_initialize();
//     sd_task_cfg_t cfg = {
//         .cs_pin = 5,
//         .data_path = "data",
//         .data_path_size = 9,
//         .spi_host = VSPI_HOST,
//         .log_path = "log",
//         .log_path_size = 5,
//         .stack_depth = 8000,
//         .priority = 0,
//         .core_id = APP_CPU_NUM,
//         .error_handler_fnc = NULL,
//     };

//     SDT_init(&cfg);
//     char dupa[256];
//     int i = 0;
//     while (1) {
//         snprintf(dupa, sizeof(dupa), "Hello world\n");
//         // ESP_LOGI(TAG, "Writing to sd");
//         SDT_send_data(dupa, sizeof(dupa));
//         snprintf(dupa, sizeof(dupa), "Hello XDDDD\n");
//         SDT_send_log(dupa, sizeof(dupa));
//         vTaskDelay(pdMS_TO_TICKS(1000));
//         i++;
//         if (i == 20) {
//             ESP_LOGI(TAG, "changing path");
//             char path[] = "test123";
//             SDT_change_data_path(path, sizeof(path));
//         }
//         if (i == 40) {
//             ESP_LOGI(TAG, "TERMINATING");
//             SDT_terminate_task();
//         }
//     }
// }

// typedef struct DataToObc {
//     bool wakenUp : 1;
//     uint32_t uptime;
//     // IMPORTANT! To implementation of each module:

//     // IMPORTANT END!
// } DataToObc;

// static void on_rec(uint8_t *data, size_t len) {
//     ESP_LOGI(TAG, "RECEIVED MESSAGE :D");
//     DataToObc x;
//     memcpy(&x, data, len);

//     ESP_LOGI(TAG, "RECEIVED %d %d", x.wakenUp, x.uptime);
// }
/*
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

void app_main(void) {
    init_state_machine();
    vTaskDelay(pdMS_TO_TICKS(10000));
    SM_change_state(IDLE);

    while (1) {
        ESP_LOGI(TAG, "Hello world! 123 %d", CONFIG_TEST);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
*/

// static void on_err(ENA_ERROR err) { ESP_LOGE(TAG, "ERROR :C %d", err); }

// const ENA_device_t test_dev = {
//     .peer = {.peer_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, .channel = 0},
//     .on_receive = on_rec,
// };

// const ENA_device_t test_dev2 = {
//     .peer = {.peer_addr = {0x78, 0x21, 0x84, 0x8d, 0x7e, 0xd0}, .channel = 0},
//     .on_receive = on_rec,
// };

// void app_main(void) {
//   uint8_t mac_address[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20};
//   ENA_config_t cfg = {
//       .stack_depth = 8000,
//       .priority = 3,
//       .core_id = APP_CPU_NUM,
//   };
//   ENA_init(mac_address);
//   ENA_register_device(&test_dev);
//   ENA_register_device(&test_dev2);
//   ENA_register_error_handler(on_err);
//   ENA_run(&cfg);
// uint8_t x = 1;
//     ENA_send(&test_dev, &x, sizeof(x), 0);
//     vTaskDelay(10000 / portTICK_PERIOD_MS);
//   while (1) {
//     // ESP_LOGI(TAG, "Hello world! 1234");
//     x = 4;
//     // ENA_send(&test_dev2, &x, sizeof(x), 3);
//     ENA_send(&test_dev, &x, sizeof(x), 0);
//     vTaskDelay(500 / portTICK_PERIOD_MS);
//   }
// }


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
