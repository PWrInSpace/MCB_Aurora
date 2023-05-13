// Copyright 2022 PWrInSpace, Kuba
#include "console_config.h"
#include "console.h"
#include "flash_task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "gen_pysd.h"
#include "flash.h"
#include "state_machine.h"
#include "../esp_now/esp_now_api.h"
#include "../esp_now/esp_now_config.h"
#define TAG "CONSOLE_CONFIG"

typedef struct DataFromObc
{
    uint32_t commandNum;
    int32_t commandArg;

} data_from_obc_t;

static int tanwa_interface_tank_esp_now(int argc, char **argv){

    data_from_obc_t data_cmd = {0x54, 0x11};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}
static ENA_device_t mac_adress_cmd;
static int send_command(int argc, char **argv){

    data_from_obc_t data_cmd;

    char tanwa_cmp[5] = "tanwa";
    char recov_cmp[5] = "recov";
    char mainv_cmp[5] = "mainv";
    char ventv_cmp[5] = "ventv";
    char pitot_cmp[5] = "pitot";


    if(!memcmp(&tanwa_cmp,argv[1],5)){

        mac_adress_cmd = esp_now_tanwa;

    } else if(!memcmp(&mainv_cmp,argv[1],5)){

        mac_adress_cmd = esp_now_main_valve;

    } else if(!memcmp(&ventv_cmp,argv[1],5)){

        mac_adress_cmd = esp_now_vent_valve;

    } else if(!memcmp(&pitot_cmp,argv[1],5)){

        mac_adress_cmd = esp_now_pitot;
        CONSOLE_WRITE_G("Sending to pitot:\n"); 

    } else if(!memcmp(&recov_cmp,argv[1],5)){

        mac_adress_cmd = esp_now_recovery;

    }

    data_cmd.commandNum = (uint32_t) strtol(argv[2],NULL,16);
    CONSOLE_WRITE_G("COMMAND NUM: %d", data_cmd.commandNum); 

    data_cmd.commandArg = (int32_t) atoi(argv[3]);
    CONSOLE_WRITE_G("COMMAND arg: %d", data_cmd.commandArg); 

    ENA_send(&mac_adress_cmd, (uint8_t *) &data_cmd, sizeof(data_cmd), 3);

    return 0;

}

static int test_command(int argc, char **argv) {

    CONSOLE_WRITE_G("i recived %d", argc,argv[0],argv[1]);
    CONSOLE_WRITE_G("First argument %s", argv[0]);
    if(argc >= 2){
        CONSOLE_WRITE_G("Second argument %s", argv[1]);
    }
    return 0;
}

//############## MAIN VALVE ######################


static int main_valve_open(int argc, char **argv){

    data_from_obc_t data_cmd = {0x01, 0x00};

    ENA_send(&esp_now_main_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int main_valve_close(int argc, char **argv){

    data_from_obc_t data_cmd = {0x00, 0x00};

    ENA_send(&esp_now_main_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;
}

static int main_valve_open_angle(int argc, char **argv){

    data_from_obc_t data_cmd = {0x03, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_main_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int main_valve_calibrate(int argc, char **argv){

    data_from_obc_t data_cmd = {0x04, 0x00};

    ENA_send(&esp_now_main_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

//################################################
//############## VENT VALVE ######################
//################################################

static int vent_valve_close(int argc, char **argv){

    data_from_obc_t data_cmd = {0x00, 0x00};

    ENA_send(&esp_now_vent_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;
}

static int vent_valve_open(int argc, char **argv){

    data_from_obc_t data_cmd = {0x01, 0x00};

    ENA_send(&esp_now_vent_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int vent_valve_open_angle(int argc, char **argv){

    data_from_obc_t data_cmd = {0x03, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_vent_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int vent_valve_autopress_time(int argc, char **argv){

    data_from_obc_t data_cmd = {0x13, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_vent_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int vent_valve_autopress_limit(int argc, char **argv){

    data_from_obc_t data_cmd = {0x14, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_vent_valve, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

//################################################
//############## RECOVERY  #######################
//################################################

static int recovery_arm_easymini(int argc, char **argv){

    data_from_obc_t data_cmd = {0x01, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int recovery_disarm_easymini(int argc, char **argv){

    data_from_obc_t data_cmd = {0x02, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int recovery_arm_telemetrum(int argc, char **argv){

    data_from_obc_t data_cmd = {0x03, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int recovery_disarm_telemetrum(int argc, char **argv){

    data_from_obc_t data_cmd = {0x04, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int recovery_force_first_separation(int argc, char **argv){

    data_from_obc_t data_cmd = {0x05, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int recovery_force_second_separation(int argc, char **argv){

    data_from_obc_t data_cmd = {0x5a, 0x00};

    ENA_send(&esp_now_recovery, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

//################################################
//################## TANWA #######################
//################################################

static int tanwa_state_change(int argc, char **argv){

    data_from_obc_t data_cmd = {0x00, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_abort(int argc, char **argv){

    data_from_obc_t data_cmd = {0x01, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_hold_in(int argc, char **argv){

    data_from_obc_t data_cmd = {0x02, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_hold_out(int argc, char **argv){

    data_from_obc_t data_cmd = {0x03, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_fill_open(int argc, char **argv){

    data_from_obc_t data_cmd = {0x10, 0x01};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_fill_close(int argc, char **argv){

    data_from_obc_t data_cmd = {0x10, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_fill_time_open(int argc, char **argv){

    data_from_obc_t data_cmd = {0x15, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_depr_open(int argc, char **argv){

    data_from_obc_t data_cmd = {0x20, 0x01};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_depr_close(int argc, char **argv){

    data_from_obc_t data_cmd = {0x20, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_qd_push(int argc, char **argv){

    data_from_obc_t data_cmd = {0x30, 0x01};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_qd_pull(int argc, char **argv){

    data_from_obc_t data_cmd = {0x30, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_soft_arm(int argc, char **argv){

    data_from_obc_t data_cmd = {0x04, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_soft_disarm(int argc, char **argv){

    data_from_obc_t data_cmd = {0x05, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_fire(int argc, char **argv){

    data_from_obc_t data_cmd = {0x88, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_soft_restart_esp(int argc, char **argv){

    data_from_obc_t data_cmd = {0x06, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_soft_restart_stm(int argc, char **argv){

    data_from_obc_t data_cmd = {0x07, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_calibrate_rck(int argc, char **argv){

    data_from_obc_t data_cmd = {0x40, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_tare_rck(int argc, char **argv){

    data_from_obc_t data_cmd = {0x41, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_set_cal_factor_rck(int argc, char **argv){

    data_from_obc_t data_cmd = {0x42, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_set_offset_rck(int argc, char **argv){

    data_from_obc_t data_cmd = {0x43, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_calibrate_tank(int argc, char **argv){

    data_from_obc_t data_cmd = {0x50, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_tare_tank(int argc, char **argv){

    data_from_obc_t data_cmd = {0x51, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_set_cal_factor_tank(int argc, char **argv){

    data_from_obc_t data_cmd = {0x52, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_set_offset_tank(int argc, char **argv){

    data_from_obc_t data_cmd = {0x53, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_plss(int argc, char **argv){

    data_from_obc_t data_cmd = {0x60, 0x00};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_interface_rck_esp_now(int argc, char **argv){

    data_from_obc_t data_cmd = {0x44, 0x11};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_interface_rck_can(int argc, char **argv){

    data_from_obc_t data_cmd = {0x44, 0x12};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}


static int tanwa_interface_tank_can(int argc, char **argv){

    data_from_obc_t data_cmd = {0x54, 0x12};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_interface_mcu_i2c(int argc, char **argv){

    data_from_obc_t data_cmd = {0x70, 0x11};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_interface_mcu_can(int argc, char **argv){

    data_from_obc_t data_cmd = {0x70, 0x12};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_lora_freq_change(int argc, char **argv){

    data_from_obc_t data_cmd = {0x80, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}

static int tanwa_lora_period_change(int argc, char **argv){

    data_from_obc_t data_cmd = {0x81, (uint16_t) atoi(argv[1])};

    ENA_send(&esp_now_tanwa, (uint8_t *) &data_cmd, sizeof(data_cmd), 1);
    return 0;

}


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
    {"state-get", "get current state", NULL, get_state, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL},
    {"test-command","test command", NULL, test_command, NULL},

    //Main Valve commands
    {"main-valve-open", "main valve open", NULL, main_valve_open, NULL},
    {"main-valve-close", "main valve close", NULL, main_valve_close, NULL},
    {"main-valve-open-angle", "main valve witch deserted angle in degrees\nmain_valve_open_angle(uint8_t angle)", NULL, main_valve_open_angle, NULL},
    {"main-valve-calibrate", "main valve close", NULL, main_valve_calibrate, NULL},

    //Vent Valve commands
    {"vent-valve-open", "vent valve open", NULL, vent_valve_open, NULL},
    {"vent-valve-close", "vent valve close", NULL, vent_valve_close, NULL},
    {"vent-valve-open-angle", "vent valve witch deserted angle in degrees\nvent_valve_open_angle(uint8_t angle)", NULL, vent_valve_open_angle, NULL},
    {"vent-valve-autopress_time", "vent valve autopress time", NULL, vent_valve_autopress_time, NULL},
    {"vent-valve-autopress_limit", "vent valve autopress limit", NULL, vent_valve_autopress_limit, NULL},

    //Recovery commands
    {"recovery-arm-easymini", "Arming easymini and backup recovery", NULL, recovery_arm_easymini, NULL},
    {"recovery-disarm-easymini", "Disarming easymini and backup recovery", NULL, recovery_disarm_easymini, NULL},
    {"recovery-arm-telemetrum", "Arming telemetrum", NULL, recovery_arm_telemetrum, NULL},
    {"recovery-disarm-telemetrum", "Disarming telemetrum", NULL, recovery_disarm_telemetrum, NULL},
    {"recovery-force-first-separation", "Forces first separation", NULL, recovery_force_first_separation, NULL},
    {"recovery-force-second-separation", "Forces second separation", NULL, recovery_force_second_separation, NULL},

    //Tanwa commands
    {"tanwa-state-change", "changes the state of tanawa module", NULL, tanwa_state_change, NULL},
    {"tanwa-abort", "tanwa abort (no argument)", NULL, tanwa_abort, NULL},
    {"tanwa-hold-in", "tanwa hold in (no argument)", NULL, tanwa_hold_in, NULL},
    {"tanwa-hold-out", "tanwa hold out (no argument)", NULL, tanwa_hold_out, NULL},
    {"tanwa-fill-open", "tanwa fill open (no argument)", NULL, tanwa_fill_open, NULL},
    {"tanwa-fill-close", "tanwa fill close (no argument)", NULL, tanwa_fill_close, NULL},
    {"tanwa-fill-time-open", "tanwa fill time open (argument time in ms)", NULL, tanwa_fill_time_open, NULL},
    {"tanwa-depr-open", "tanwa depr open (no argument)", NULL, tanwa_depr_open, NULL},
    {"tanwa-depr-close", "tanwa depr close (no argument)", NULL, tanwa_depr_close, NULL},
    {"tanwa-qd-push", "tanwa qd push (no argument)", NULL, tanwa_qd_push, NULL},
    {"tanwa-qd-pull", "tanwa qd pull (no argument)", NULL, tanwa_qd_pull, NULL},
    {"tanwa-soft-arm", "tanwa soft arm (no argument)", NULL, tanwa_soft_arm, NULL},
    {"tanwa-soft-disarm", "tanwa soft disarm (no argument)", NULL, tanwa_soft_disarm, NULL},
    {"tanwa-fire", "tanwa fire (no argument)", NULL, tanwa_fire, NULL},
    {"tanwa-soft-restart-esp", "tanwa soft restart esp (no argument)", NULL, tanwa_soft_restart_esp, NULL},
    {"tanwa-soft-restart-stm", "tanwa soft restart stm (no argument)", NULL, tanwa_soft_restart_stm, NULL},
    {"tanwa-calibrate-rck", "tanwa calibrate rck (weight in grams)", NULL, tanwa_calibrate_rck, NULL},
    {"tanwa-tare-rck", "tanwa tare rck (no argument)", NULL, tanwa_tare_rck, NULL},
    {"tanwa-set-cal-factor-rck", "tanwa set cal factor rck (factor)", NULL, tanwa_set_cal_factor_rck, NULL},
    {"tanwa-set-offset-rck", "tanwa set offset rck (weight in grams)", NULL, tanwa_set_offset_rck, NULL},
    {"tanwa-calibrate-tank", "tanwa calibrate tank (weight in grams)", NULL, tanwa_calibrate_tank, NULL},
    {"tanwa-tare-tank", "tanwa tare tank (no argument)", NULL, tanwa_tare_tank, NULL},
    {"tanwa-set-cal-factor-tank", "tanwa set cal factor tank (factor)", NULL, tanwa_set_cal_factor_tank, NULL},
    {"tanwa-set-offset-tank", "tanwa set offset tank (weight in grams)", NULL, tanwa_set_offset_tank, NULL},
    {"tanwa-plss", "tanwa plss (no argument)", NULL, tanwa_plss, NULL},
    {"tanwa-interface-rck-esp-now", "tanwa interface rck esp now (no argument)", NULL, tanwa_interface_rck_esp_now, NULL},
    {"tanwa-intefrace-rck-can", "tanwa interface rck can (no argument)", NULL, tanwa_interface_rck_can, NULL},
    {"tanwa-interface-tank-esp-now", "tanwa interface tank esp now (no argument)", NULL, tanwa_interface_tank_esp_now, NULL},
    {"tanwa-intefrace-tank-can", "tanwa interface tank can (no argument)", NULL, tanwa_interface_tank_can, NULL},
    {"tanwa-interface-mcu-i2c", "tanwa interface mcu i2c (no argument)", NULL, tanwa_interface_mcu_i2c, NULL},
    {"tanwa-intefrace-mcu-can", "tanwa interface mcu can (no argument)", NULL, tanwa_interface_mcu_can, NULL},
    {"tanwa-lora-freq-change", "tanwa frequency change (khz)", NULL, tanwa_lora_freq_change, NULL},
    {"tanwa-lora-period-change", "tanwa period change (ms)", NULL, tanwa_lora_period_change, NULL},


    {"send-command", "\nsend-command(string module, uint32_t command_num, int32_t command_arg)\nList of modules:\npitot\nmainv\nventv\ntanwa\nrecov", NULL, send_command, NULL},
};

esp_err_t init_console() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd)/sizeof(cmd[0]));
    // ESP_LOGW(TAG, "%s", esp_err_to_name(ret));
    return ret;
}