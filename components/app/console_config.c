// Copyright 2022 PWrInSpace, Kuba
#include "console_config.h"

#include "commands.h"
#include "commands_config.h"
#include "console.h"
#include "esp_log.h"
#include "esp_now_config.h"
#include "esp_system.h"
#include "flash.h"
#include "flash_task.h"
#include "gen_pysd.h"
#include "mission_timer_config.h"
#include "nvs_config.h"
#include "state_machine_config.h"
#include "system_timer_config.h"
#define TAG "CONSOLE_CONFIG"

static int read_flash(int argc, char **argv) {
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

static int reset_device(int argc, char **arg) {
    esp_restart();
    return 0;
}

static int flash_start(int argc, char **arg) {
    FT_start_loop();
    return 0;
}

static int flash_terminate(int argc, char **arg) {
    FT_terminate_task();
    return 0;
}

static int change_state(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    cmd_message_t command = cmd_create_message(MCB_STATE_CHANGE, atoi(argv[1]));
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }

    return 0;
}

static int force_change_state(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    int state = atoi(argv[1]);
    if (state == 11) {
        if (SM_get_current_state() == HOLD) {
            ESP_LOGI(TAG, "Leaving hold state");
            if (SM_get_previous_state() == COUNTDOWN) {
                SM_force_change_state(RDY_TO_LAUNCH);
            } else {
                SM_change_to_previous_state(true);
            }
        } else {
            SM_force_change_state(HOLD);
            ESP_LOGI(TAG, "HOLD");
        }
        return 0;
    }
    if (SM_force_change_state(state) != SM_OK) {
        return -1;
    }

    return 0;
}

static int change_to_previous_state(int argc, char **argv) {
    bool run_callback = false;
    if (argc == 2) {
        run_callback = true;
    }

    if (SM_change_to_previous_state(run_callback) != SM_OK) {
        return -1;
    }

    return 0;
}

static int get_state(int argc, char **argv) {
    CONSOLE_WRITE_G("Current state -> %d", SM_get_current_state());
    return 0;
}

static int disable_log(int argc, char **argv) {
    if (argc == 2) {
        esp_log_level_set(argv[1], ESP_LOG_WARN);
    } else {
        esp_log_level_set("*", ESP_LOG_WARN);
    }

    return 0;
}

static int enable_log(int argc, char **argv) {
    if (argc == 2) {
        esp_log_level_set(argv[1], ESP_LOG_DEBUG);
    } else {
        esp_log_level_set("*", ESP_LOG_DEBUG);
    }
    esp_log_level_set("spi_master", ESP_LOG_ERROR);
    return 0;
}

static int reset_dc_timer(int argc, char **argv) {
    cmd_message_t command = cmd_create_message(MCB_RESET_DISCONNECT_TIMER, 0);
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }
    return 0;
}

static int esp_now_send_tanwa(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_tanwa, msg.raw, sizeof(msg.raw), 3);

    return 0;
}

static int esp_now_send_main_valve(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_main_valve, msg.raw, sizeof(msg.raw), 3);
    return 0;
}

static int cli_settings_read_all(int argc, char **argv) {
    int32_t lora_Freq_KHz;
    int32_t lora_transmit_ms;
    int32_t  countdownTime;
    int32_t igniteTime;
    int32_t buzzer_on;
    int32_t flash_on;
    NVS_read_int32t(SETTINGS_LORA_FREQ_KHZ,&lora_Freq_KHz);
    NVS_read_int32t(SETTINGS_LORA_TRANSMIT_MS,&lora_transmit_ms);
    NVS_read_int32t(SETTINGS_COUNTDOWN_TIME,&countdownTime);
    NVS_read_int32t(SETTINGS_IGNITE_TIME,&igniteTime);
    NVS_read_int32t(SETTINGS_BUZZER_ON,&buzzer_on);
    NVS_read_int32t(SETTINGS_FLASH_ON,&flash_on);
    CONSOLE_WRITE("Lora freq MHZ %d", lora_Freq_KHz);
    CONSOLE_WRITE("Lora transmit freq %d", lora_transmit_ms);
    CONSOLE_WRITE("CDWN TIME %d", countdownTime);
    CONSOLE_WRITE("Igni time %d", igniteTime);
    CONSOLE_WRITE("Buzzer %d", buzzer_on);
    CONSOLE_WRITE("Flash %d", flash_on);

    return 0;
}

static int cli_settings_init_default(int argc, char **argv) {
    default_nvs();
    return 0;
}

static int cli_change_countdown_time(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    cmd_message_t command = cmd_create_message(MCB_CHANGE_COUNTODWN_TIME, atoi(argv[1]));
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }

    return 0;
}

static int cli_change_ignition_time(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    cmd_message_t command = cmd_create_message(MCB_CHANGE_IGNITION_TIME, atoi(argv[1]));
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }

    return 0;
}

static int cli_change_lora_transmiting_period(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    cmd_message_t command = cmd_create_message(MCB_CHANGE_TX_PERIOD, atoi(argv[1]));
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }

    return 0;
}

static int cli_change_lora_frequency(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    cmd_message_t command = cmd_create_message(MCB_CHANGE_LORA_FREQ, atoi(argv[1]));
    if (lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command) == false) {
        return -1;
    }

    return 0;
}

static esp_console_cmd_t cmd[] = {
    {"flash-read", "Read data from flash memory", NULL, read_flash, NULL},
    {"reset-dev", "Restart device", NULL, reset_device, NULL},
    {"flash-start", "start flash task loop", NULL, flash_start, NULL},
    {"flash-terminate", "terminate flash loop", NULL, flash_terminate, NULL},
    {"state-change", "change state", NULL, change_state, NULL},
    {"state-change-prev", "change state to previous", NULL, change_to_previous_state, NULL},
    {"state-change-force", "force change state", NULL, force_change_state, NULL},
    {"state-get", "get current state", NULL, get_state, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL},
    {"reset-dc", "reset disconnect timer", NULL, reset_dc_timer, NULL},
    {"en_tanwa", "send command to tanwa", NULL, esp_now_send_tanwa, NULL},
    {"en_mv", "send command to main valve", NULL, esp_now_send_main_valve, NULL},
    {"settings_all", "get all settings", NULL, cli_settings_read_all, NULL},
    {"settings_init", "init settings default", NULL, cli_settings_init_default, NULL},
    {"settings_ignit", "change ignition time", NULL, cli_change_ignition_time, NULL},
    {"settings_cdwn", "change countdown time", NULL, cli_change_countdown_time, NULL},
    {"lora_period", "change lora transmit period ms", NULL,
     cli_change_lora_transmiting_period, NULL},
    {"lora_frequency", "change lora frerquency ms", NULL,
     cli_change_lora_frequency, NULL},
};

esp_err_t init_console() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
    // ESP_LOGW(TAG, "%s", esp_err_to_name(ret));
    return ret;
}