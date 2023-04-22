// Copyright 2022 PWrInSpace, Kuba
#include "console_config.h"
#include "console.h"
#include "flash_task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "gen_pysd.h"
#include "flash.h"
#include "state_machine.h"
#define TAG "CONSOLE_CONFIG"

static int read_flash(int argc, char** argv) {
    esp_log_level_set("*", ESP_LOG_NONE);
    FILE *file = NULL;
    file = fopen(FLASH_PATH, "r");
    if (file == NULL) {
        CONSOLE_WRITE_E("Unable to open file");
        return -1;
    }

    char buff[512];
    rocket_data_t data;
    while (fread(&data, sizeof(data), 1, file)) {
        pysd_create_sd_frame(buff, sizeof(buff), data, false);
        CONSOLE_WRITE("%s", buff);
    }
    fclose(file);

    CONSOLE_WRITE_G("Read end");
    esp_log_level_set("*", ESP_LOG_INFO);
    return 0;
}

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

static int change_state(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    int state = atoi(argv[1]);
    if (SM_change_state(state) != SM_OK) {
        return -1;
    }

    return 0;
}

static int force_change_state(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    int state = atoi(argv[1]);
    if (SM_force_change_state(state) != SM_OK) {
        return -1;
    }

    return 0;
}

static int get_state(int argc, char **argv) {
    CONSOLE_WRITE_G("Current state -> %d", SM_get_current_state());
    return 0;
}

static int disable_log(int argc, char **argv) {
    esp_log_level_set("*", ESP_LOG_NONE);
    return 0;
}

static int enable_log(int argc, char **argv) {
    esp_log_level_set("*", ESP_LOG_INFO);
    return 0;
}

static esp_console_cmd_t cmd[] = {
    {"flash-read", "Read data from flash memory", NULL, read_flash, NULL},
    {"reset-dev", "Restart device", NULL, reset_device, NULL},
    {"flash-start", "start flash task loop", NULL, flash_start, NULL},
    {"flash-terminate", "terminate flash loop", NULL, flash_terminate, NULL},
    {"state-change", "change state", NULL, change_state, NULL},
    {"state-change-force", "force change state", NULL, force_change_state, NULL},
    {"state-get", "get current state", NULL, get_state, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL},
};

esp_err_t init_console() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd)/sizeof(cmd[0]));
    // ESP_LOGW(TAG, "%s", esp_err_to_name(ret));
    return ret;
}