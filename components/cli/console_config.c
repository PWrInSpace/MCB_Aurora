// Copyright 2022 PWrInSpace, Kuba
#include "console_config.h"
#include "console.h"
#include "flash_task.h"
#include "esp_log.h"
#include "esp_system.h"

#define TAG "CONSOLE_CONFIG"

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

static int reset_device(int argc, char** arg) {
    esp_restart();
    return 0;
}

static int flash_start(int argc, char** arg ) {
    FT_erase_and_run_loop();
    return 0;
}

static int flash_terminate(int argc, char ** arg) {
    FT_terminate_task();
    return 0;
}

static esp_console_cmd_t cmd[] = {
    // {"flash-read", "123", NULL, read_flash, NULL},
    {"reset-dev", "Restart device", NULL, reset_device, NULL},
    {"flash-start", "start flash task loop", NULL, flash_start, NULL},
    {"flash-terminate", "terminate flash loop", NULL, flash_terminate, NULL}
};

esp_err_t init_console() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd)/sizeof(cmd[0]));
    // ESP_LOGW(TAG, "%s", esp_err_to_name(ret));
    return ret;
}