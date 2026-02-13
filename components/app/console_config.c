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
#include "settings_mem.h"
#include "state_machine_config.h"
#include "system_timer_config.h"
#include "rocket_data.h"
#define TAG "CONSOLE_CONFIG"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

static int esp_now_send_n2_main_valve(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_n2_main_valve, msg.raw, sizeof(msg.raw), 3);
    return 0;
}

static int esp_now_send_vent_valves(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_vent_valves, msg.raw, sizeof(msg.raw), 3);
    return 0;
}

static int esp_now_send_ox_vent_eth_main_valves(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_ox_vent_eth_main_valves, msg.raw, sizeof(msg.raw), 3);
    return 0;
}

static int esp_now_send_ox_main_valve(int argc, char **argv) {
    if (argc != 3) {
        return -1;
    }

    int command = atoi(argv[1]);
    int payload = atoi(argv[2]);

    cmd_message_t msg = cmd_create_message(command, payload);
    ENA_send(&esp_now_ox_main_valve, msg.raw, sizeof(msg.raw), 3);
    return 0;
}

static int cli_settings_read_all(int argc, char **argv) {
    Settings settings = settings_get_all();
    CONSOLE_WRITE("Lora freq MHZ %d", settings.loraFreq_KHz);
    CONSOLE_WRITE("Lora transmit freq %d", settings.lora_transmit_ms);
    CONSOLE_WRITE("CDWN TIME %d", settings.countdownTime);
    CONSOLE_WRITE("Igni time %d", settings.ignitTime);
    CONSOLE_WRITE("Buzzer %d", settings.buzzer_on);
    CONSOLE_WRITE("Flash %d", settings.flash_on);

    return 0;
}

static int cli_settings_init_default(int argc, char **argv) {
    settings_init_default();
    settings_read_all();

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
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_MCB, &command);

    return 0;
}

int recovery_force_first_stage(int argc, char **argv) {

    cmd_message_t command = cmd_create_message(RECOV_FORCE_FIRST_STAGE, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int recovery_force_second_stage(int argc, char **argv) {

    cmd_message_t command = cmd_create_message(RECOV_FORCE_SECOND_STAGE, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int recovery_easymini_arm(int argc, char **argv) {

    cmd_message_t command = cmd_create_message(RECOV_EASYMINI_ARM, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int recovery_easymini_disarm(int argc, char **argv) {

    cmd_message_t command = cmd_create_message(RECOV_EASYMINI_DISARM, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int recovery_telemetrum_arm(int argc, char **argv) {


    cmd_message_t command = cmd_create_message(RECOV_TELEMETRUM_ARM, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int recovery_telemetrum_disarm(int argc, char **argv) {
    

    cmd_message_t command = cmd_create_message(RECOV_TELEMETRUM_DISARM, 0);
    lora_cmd_process_command(LORA_DEV_ID, DEVICE_RECOVERY, &command);

    return 0;
}

int get_data(int argc, char **argv) {
    
    mcb_data_t data = rocket_data_get_mcb();

    CONSOLE_WRITE("State: %d", data.state);
    CONSOLE_WRITE("Battery voltage: %.2f", data.battery_voltage);
    CONSOLE_WRITE("GPS altitude: %.2f", data.gps_altitude);    
    CONSOLE_WRITE("Altitude: %.2f", data.altitude);
    CONSOLE_WRITE("Velocity: %.2f", data.velocity);
    return 0;
}

static esp_console_cmd_t cmd[] = {
    {"flash-read", "Read data from flash memory", NULL, read_flash, NULL, NULL, NULL},
    {"reset-dev", "Restart device", NULL, reset_device, NULL, NULL, NULL},
    {"flash-start", "start flash task loop", NULL, flash_start, NULL, NULL, NULL},
    {"flash-terminate", "terminate flash loop", NULL, flash_terminate, NULL, NULL, NULL},
    {"state-change", "change state", NULL, change_state, NULL, NULL, NULL},
    {"state-change-prev", "change state to previous", NULL, change_to_previous_state, NULL, NULL, NULL},
    {"state-change-force", "force change state", NULL, force_change_state, NULL, NULL, NULL},
    {"state-get", "get current state", NULL, get_state, NULL, NULL, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL, NULL, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL, NULL, NULL},
    {"reset-dc", "reset disconnect timer", NULL, reset_dc_timer, NULL, NULL, NULL},
    {"en_tanwa", "send command to tanwa", NULL, esp_now_send_tanwa, NULL, NULL, NULL},
    {"en_n2_mv", "send command to n2 main valve", NULL, esp_now_send_n2_main_valve, NULL, NULL, NULL},
    {"en_vv", "send command to vent valves", NULL, esp_now_send_vent_valves, NULL, NULL, NULL},
    {"en_ox_mv_eth_vv", "send command to ox main, eth vent valves", NULL, esp_now_send_ox_vent_eth_main_valves, NULL, NULL, NULL},
    {"en_ox_mv", "send command to ox main valve", NULL, esp_now_send_ox_main_valve, NULL, NULL, NULL},
    {"settings_all", "get all settings", NULL, cli_settings_read_all, NULL, NULL, NULL},
    {"settings_init", "init settings default", NULL, cli_settings_init_default, NULL, NULL, NULL},
    {"settings_ignit", "change ignition time", NULL, cli_change_ignition_time, NULL, NULL, NULL},
    {"settings_cdwn", "change countdown time", NULL, cli_change_countdown_time, NULL, NULL, NULL},
    {"lora_period", "change lora transmit period ms", NULL,
     cli_change_lora_transmiting_period, NULL, NULL, NULL},
    {"lora_frequency", "change lora frerquency ms", NULL,
     cli_change_lora_frequency, NULL, NULL, NULL},
    {"recov_force_first", "force first stage separation", NULL, recovery_force_first_stage, NULL, NULL, NULL},
    {"recov_force_second", "force second stage separation", NULL, recovery_force_second_stage, NULL, NULL, NULL},
    {"recov_easymini_arm", "arm easymini recovery", NULL, recovery_easymini_arm, NULL, NULL, NULL},
    {"recov_easymini_disarm", "disarm easymini recovery", NULL, recovery_easymini_disarm, NULL, NULL, NULL},
    {"recov_telemetrum_arm", "arm telemetrum recovery", NULL, recovery_telemetrum_arm, NULL, NULL, NULL},
    {"recov_telemetrum_disarm", "disarm telemetrum recovery", NULL, recovery_telemetrum_disarm, NULL, NULL, NULL},
    {"get_data", "get mcb data", NULL, get_data, NULL, NULL, NULL},
};

static void console_register_task(void *arg) {
    (void)arg;
    esp_err_t ret = ESP_OK;
    /* small delay to allow other init actions to settle */
    vTaskDelay(pdMS_TO_TICKS(50));
    ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
    if (ret == ESP_OK) {
        ret = console_start();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "console_start failed: %s", esp_err_to_name(ret));
        }
    } else {
        ESP_LOGW(TAG, "console_register_commands failed: %s", esp_err_to_name(ret));
    }
    vTaskDelete(NULL);
}

esp_err_t init_console() {
    esp_err_t ret = console_init();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "console_init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    BaseType_t tr = xTaskCreate(console_register_task, "console_reg", 4096, NULL, 5, NULL);
    if (tr != pdPASS) {
        ESP_LOGE(TAG, "Failed to create console register task");
        return ESP_FAIL;
    }
    return ESP_OK;
}