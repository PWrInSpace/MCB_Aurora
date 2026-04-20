#include "errors_config.h"
#include "esp_log.h"
#include "lora.pb-c.h"
#include "math.h"
#include "rocket_data.h"
#include "settings_mem.h"
#include "slave_structs.h"

#define TAG "PBF"

void create_protobuf_data_frame(struct obc_lo_ra_frame_t *frame) {
    rocket_data_t data = rocket_data_get();
    error_data_t errors[MAX_NUMBER_OF_ERRORS];
    if (errors_get_all(errors, sizeof(errors)) == false) {
        ESP_LOGI(TAG, "Unable to get errors");
    }

    // mcb
    frame->stan_mcb.is_present = true;
    frame->stan_mcb.value = data.mcb.state;

    frame->uptime_disconnect_timer.is_present = true;
    frame->uptime_disconnect_timer.value = data.mcb.disconnect_timer;

    frame->czas_lotu.is_present = true;
    frame->czas_lotu.value = data.mcb.flight_time;

    frame->mcb_batt.is_present = true;
    frame->mcb_batt.value = data.mcb.battery_voltage;

    frame->gps_lat.is_present = true;
    frame->gps_lat.value = data.mcb.latitude;

    frame->gps_long.is_present = true;
    frame->gps_long.value = data.mcb.longitude;

    // gps_sat in proto is a bool — set true if number of satellites > 0
    frame->gps_sat.is_present = true;
    frame->gps_sat.value = data.mcb.satelites_in_view > 0;

    frame->altitude.is_present = true;
    frame->altitude.value = (int32_t)data.mcb.altitude;

    frame->velocity.is_present = true;
    frame->velocity.value = (int32_t)data.mcb.velocity;

    frame->mcb_temperature.is_present = true;
    frame->mcb_temperature.value = (int32_t)data.mcb.temperature;

    frame->euler_psi.is_present = true;
    frame->euler_psi.value = data.mcb.pitch;

    frame->euler_theta.is_present = true;
    frame->euler_theta.value = data.mcb.roll;

    frame->euler_fi.is_present = true;
    frame->euler_fi.value = data.mcb.yaw;

    // recovery - pack various boolean fields into a single fixed32 recovery_flags
    {
        uint32_t recovery_flags = 0;
        recovery_flags |= data.recovery.separationSwitch1 ? 1u << 0 : 0u;
        recovery_flags |= data.recovery.separationSwitch2 ? 1u << 1 : 0u;
        recovery_flags |= data.recovery.firstStageDone ? 1u << 2 : 0u;
        recovery_flags |= data.recovery.secondStageDone ? 1u << 3 : 0u;
        recovery_flags |= data.recovery.telemetrumFirstStage ? 1u << 4 : 0u;
        recovery_flags |= data.recovery.telemetrumSecondStage ? 1u << 5 : 0u;
        recovery_flags |= data.recovery.easyMiniFirstStage ? 1u << 6 : 0u;
        recovery_flags |= data.recovery.easyMiniSecondStage ? 1u << 7 : 0u;
        recovery_flags |= data.recovery.isTeleActive ? 1u << 8 : 0u;
        recovery_flags |= data.recovery.isArmed ? 1u << 9 : 0u;
        //ESP_LOGI(TAG, "arm status: %d", data.recovery.isArmed);
        //ESP_LOGI(TAG, "telemetry status: %d", data.recovery.isTeleActive);
        frame->recovery_flags.is_present = true;
        frame->recovery_flags.value = recovery_flags;
    }

    // pitot
    frame->pitot_bateria.is_present = true;
    frame->pitot_bateria.value = data.pitot.vbat;

    frame->pitot_wysokosc.is_present = true;
    frame->pitot_wysokosc.value = (int32_t)data.pitot.alt;

    frame->pitot_predkosc.is_present = true;
    frame->pitot_predkosc.value = (int32_t)data.pitot.speed;

    frame->pitot_temperatura.is_present = true;
    frame->pitot_temperatura.value = (int32_t)data.pitot.temperature;

    uint32_t valve_states_bitfield = 0;
    valve_states_bitfield |= data.ox_main_valve.valve_1_state == 1 ? 1u << 0 : 0u;
    valve_states_bitfield |= data.ox_vent_eth_main_valves.valve_1_state == 1 ? 1u << 2 : 0u;
    valve_states_bitfield |= data.ox_vent_eth_main_valves.valve_2_state == 1 ? 1u << 3 : 0u;
    valve_states_bitfield |= data.eth_vent_valve.valve_1_state == 1 ? 1u << 4 : 0u;
    valve_states_bitfield |= data.n2_vent_valve.valve_1_state == 1 ? 1u << 5 : 0u;

    frame->main_vent_flags.is_present = true;
    frame->main_vent_flags.value = valve_states_bitfield;

    // main valve
    // mval bit data mapping not present in proto; skipping

    // Pack valve bit-data fields using lowest-index sensors as requested:
    // Byte layout: [temp1 (8b)] [pressure1 high byte (8b)] [pressure2 high byte (8b)] [battery scaled (8b)]

    // ox vent eth main bit data (uses ox_vent_eth_main_valves struct)
    {
        uint32_t v = 0;
        uint16_t pressure = (uint16_t)(fmax(data.ox_vent_eth_main_valves.pressure_1, 0) * 100);
        v |= (uint32_t)pressure << 16;
        uint8_t battery_voltage = (uint8_t)fminf(255.0f, data.ox_vent_eth_main_valves.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 8;
        uint8_t battery_consumption = (uint8_t)fminf(255.0f, data.ox_vent_eth_main_valves.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 0;
        frame->ox_vent_eth_main_bit_data_a.is_present = true;
        frame->ox_vent_eth_main_bit_data_a.value = v;
    }

    {
        uint32_t v = 0;
        v |= (uint32_t)data.ox_vent_eth_main_valves.is_charging << 31;
        int8_t charger_temperature = (int8_t)fminf(255.0f, roundf(data.ox_vent_eth_main_valves.charger_temperature));
        v |= (uint32_t)charger_temperature << 23;
        uint16_t pressure = (uint16_t)(fmax(data.ox_vent_eth_main_valves.pressure_2, 0) * 100);
        v |= (uint32_t)pressure << 7;
        ESP_LOGI(TAG, "Pressure 2 packed: %d", pressure);
        frame->ox_vent_eth_main_bit_data_b.is_present = true;
        frame->ox_vent_eth_main_bit_data_b.value = v;
    }

    // ox main bit data (uses ox_main_valve struct)
    {
        uint32_t v = 0;
        uint16_t pressure = (uint16_t)(fmax(data.ox_main_valve.pressure_1, 0) * 100);
        v |= (uint32_t)pressure << 16;
        v |= (uint32_t)data.ox_main_valve.dump_valve_arm << 15;
        v |= (uint32_t)data.ox_main_valve.dump_valve_cont << 14;
        int8_t temperature = (int8_t)fminf(255.0f, roundf(data.ox_main_valve.temperature_1));
        v |= (uint32_t)temperature << 6;
        frame->ox_main_bit_data_a.is_present = true;
        frame->ox_main_bit_data_a.value = v;
    }

    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)fminf(255.0f, data.ox_main_valve.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)fminf(255.0f, data.ox_main_valve.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.ox_main_valve.is_charging << 15;
        int8_t charger_temperature = (int8_t)fminf(255.0f, roundf(data.ox_main_valve.charger_temperature));
        v |= (uint32_t)charger_temperature << 7;
        frame->ox_main_bit_data_b.is_present = true;
        frame->ox_main_bit_data_b.value = v;
    }

    // n2 vent bit data (uses n2_vent_valve struct)
    {
        uint32_t v = 0;
        uint16_t pressure_1 = (uint16_t)(fmax(data.eth_vent_valve.pressure_1, 0) * 100);
        v |= (uint32_t)pressure_1 << 16;
        frame->eth_vent_bit_data_b.is_present = true;
        frame->eth_vent_bit_data_b.value = v;
    }

    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)fminf(255.0f, data.n2_vent_valve.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)fminf(255.0f, data.n2_vent_valve.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.n2_vent_valve.is_charging << 15;
        int8_t charger_temperature = (int8_t) (fminf(255.0f, roundf(data.n2_vent_valve.charger_temperature)));
        v |= (uint32_t)charger_temperature << 7;
        frame->n2_vent_bit_data_a.is_present = true;
        frame->n2_vent_bit_data_a.value = v;
    }

    // eth vent bit data (uses eth_vent_valve struct)
    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)fminf(255.0f, data.eth_vent_valve.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)fminf(255.0f, data.eth_vent_valve.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.eth_vent_valve.is_charging << 15;
        int8_t charger_temperature = (int8_t)fminf(255.0f, roundf(data.eth_vent_valve.charger_temperature));
        v |= (uint32_t)charger_temperature << 7;
        frame->eth_vent_bit_data_a.is_present = true;
        frame->eth_vent_bit_data_a.value = v;
    }

    // tanwa - map explicit fields from tanwa_data_t
    frame->tanwa_bateria.is_present = true;
    frame->tanwa_bateria.value = data.tanwa.vbat;

    frame->tanwa_state.is_present = true;
    frame->tanwa_state.value = data.tanwa.tanWaState;

    frame->tanwa_thrust.is_present = true;
    frame->tanwa_thrust.value = data.tanwa.thrust_val;

    frame->tanwa_tank_weight.is_present = true;
    frame->tanwa_tank_weight.value = data.tanwa.tankWeight_val;

    frame->tanwa_temperature_post_n2_o_fill.is_present = true;
    frame->tanwa_temperature_post_n2_o_fill.value = data.tanwa.temperature_postFill;

    frame->tanwa_temperature_filling_wall.is_present = true;
    frame->tanwa_temperature_filling_wall.value = data.tanwa.temperature_Wall;

    frame->tanwa_post_fill_n2_o_pres.is_present = true;
    frame->tanwa_post_fill_n2_o_pres.value = data.tanwa.postFillN2O_pres;

    frame->tanwa_cutoff_n2_o_pres.is_present = true;
    frame->tanwa_cutoff_n2_o_pres.value = data.tanwa.cutoffN2O_pres;

    frame->tanwa_droid_n2_o_press.is_present = true;
    frame->tanwa_droid_n2_o_press.value = data.tanwa.droidN2O_press;

    frame->tanwa_pre_regulator_n2_pres.is_present = true;
    frame->tanwa_pre_regulator_n2_pres.value = data.tanwa.preRegulatorN2_pres;

    frame->tanwa_post_regulator_n2_pres.is_present = true;
    frame->tanwa_post_regulator_n2_pres.value = data.tanwa.postRegulatorN2_pres;

    frame->tanwa_post_fill_n2_pres.is_present = true;
    frame->tanwa_post_fill_n2_pres.value = data.tanwa.postFillN2_pres;

    frame->tanwa_droid_n2_press.is_present = true;
    frame->tanwa_droid_n2_press.value = data.tanwa.droidN2_press;

    frame->tanwa_comb_chamber_pres.is_present = true;
    frame->tanwa_comb_chamber_pres.value = data.tanwa.combChamber_pres;

    // tanwa flags - pack many boolean status bits into a single fixed32 tanwa_flags
    {
        uint32_t tanwa_flags = 0;
        tanwa_flags |= data.tanwa.canWeights_con ? 1u << 0 : 0u; // CAN_Weights_connection
        tanwa_flags |= data.tanwa.canUtility_con ? 1u << 1 : 0u; // CAN_Utility_connection
        tanwa_flags |= data.tanwa.canSensor_con ? 1u << 2 : 0u; // CAN_Sensor_connection
        tanwa_flags |= data.tanwa.canPower_con ? 1u << 3 : 0u; // CAN_Power_connection
        tanwa_flags |= data.tanwa.canSolenoid_con ? 1u << 4 : 0u; // CAN_Solenoid_connection
        tanwa_flags |= data.tanwa.igniterContinouity_1 ? 1u << 5 : 0u; // igniter_1_continuity
        tanwa_flags |= data.tanwa.igniterContinouity_2 ? 1u << 6 : 0u; // igniter_2_continuity
        tanwa_flags |= data.tanwa.soft_arm ? 1u << 7 : 0u; // soft_arm
        tanwa_flags |= data.tanwa.abortButton ? 1u << 8 : 0u; // abort_button
        tanwa_flags |= data.tanwa.fillN2OState ? 1u << 9 : 0u;
        tanwa_flags |= data.tanwa.deprN2OState ? 1u << 10 : 0u;
        tanwa_flags |= data.tanwa.fillN2State ? 1u << 11 : 0u;
        tanwa_flags |= data.tanwa.deprN2State ? 1u << 12 : 0u;
        tanwa_flags |= data.tanwa.droidN2OState ? 1u << 13 : 0u;
        tanwa_flags |= data.tanwa.droidN2State ? 1u << 14 : 0u;
        tanwa_flags |= data.tanwa.heatingTankState ? 1u << 15 : 0u;
        tanwa_flags |= data.tanwa.heatingValveState ? 1u << 16 : 0u;
        frame->tanwa_flags.is_present = true;
        frame->tanwa_flags.value = tanwa_flags;
    }
    
    // payload
    frame->payload_bateria.is_present = true;
    frame->payload_bateria.value = data.payload.vbat;

    // ESP-NOW connected flags (bitfield) - pack individual connection booleans
    {
        uint32_t conn = 0;
        conn |= data.connected_dev.payload ? 1u << 0 : 0u; // payload_connected
        conn |= data.connected_dev.tanwa ? 1u << 1 : 0u; // tanwa_connected
        conn |= data.connected_dev.eth_vent_valve ? 1u << 2 : 0u;
        conn |= data.connected_dev.ox_main_valve ? 1u << 3 : 0u;
        conn |= data.connected_dev.ox_vent_eth_main_valves ? 1u << 4 : 0u;
        conn |= data.connected_dev.n2_vent_valve ? 1u << 5 : 0u;
        conn |= data.connected_dev.pitot ? 1u << 6 : 0u;
        frame->espnow_connected_flags.is_present = true;
        frame->espnow_connected_flags.value = conn;
    }

    // ESP-NOW wakeup flags (bitfield) - pack waken_up booleans
    {
        uint32_t wk = 0;
        wk |= data.payload.waken_up ? 1u << 0 : 0u;
        wk |= data.eth_vent_valve.waken_up ? 1u << 1 : 0u;
        wk |= data.ox_main_valve.waken_up ? 1u << 2 : 0u;
        wk |= data.ox_vent_eth_main_valves.waken_up ? 1u << 3 : 0u;
        wk |= data.n2_vent_valve.waken_up ? 1u << 4 : 0u;
        wk |= data.pitot.waken_up ? 1u << 5 : 0u;
        frame->espnow_wkup_flags.is_present = true;
        frame->espnow_wkup_flags.value = wk;
    }


    // errors - pack into single uint32 value as before
    frame->errors.is_present = true;
    frame->errors.value = 0;
    frame->errors.value |= errors[ERROR_TYPE_LAST_EXCEPTION];
    frame->errors.value |= errors[ERROR_TYPE_RECOVERY] << 8;
    frame->errors.value |= errors[ERROR_TYPE_ESP_NOW] << 12;
    frame->errors.value |= errors[ERROR_TYPE_MEMORY] << 16;
    frame->errors.value |= errors[ERROR_TYPE_MCB] << 24;
    frame->errors.value |= errors[ERROR_TYPE_SENSORS] << 28;
}

void create_protobuf_settings_frame(struct obc_lo_ra_settings_t *frame) {
    Settings settings = settings_get_all();
    frame->countdown_time.value = settings.countdownTime;
    frame->ingition_time.value = settings.ignitTime;
    frame->lora_freq_khz.value = settings.loraFreq_KHz;
    frame->lora_transmit_ms.value = settings.lora_transmit_ms;
    frame->buzzer_enable.value = settings.buzzer_on;
    frame->flash_enable.value = settings.flash_on;

    frame->countdown_time.is_present = true;
    frame->ingition_time.is_present = true;
    frame->lora_freq_khz.is_present = true;
    frame->lora_transmit_ms.is_present = true;
    frame->buzzer_enable.is_present = true;
    frame->flash_enable.is_present = true;
}