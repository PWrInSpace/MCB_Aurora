#include "errors_config.h"
#include "esp_log.h"
#include "lora.pb-c.h"
#include "math.h"
#include "rocket_data.h"
#include "settings_mem.h"
#include "slave_structs.h"

#define TAG "PBF"

void create_protobuf_data_frame(struct obc_mcb_frame_t *frame) {
    rocket_data_t data = rocket_data_get();
    error_data_t errors[MAX_NUMBER_OF_ERRORS];
    if (errors_get_all(errors, sizeof(errors)) == false) {
        ESP_LOGI(TAG, "Unable to get errors");
    }

    // mcb
    frame->mcb_state.is_present = true;
    frame->mcb_state.value = data.mcb.state;

    frame->uptime_ms.is_present = true;
    frame->uptime_ms.value = data.mcb.disconnect_timer;

    frame->flight_time_ms.is_present = true;
    frame->flight_time_ms.value = data.mcb.flight_time;

    frame->mcb_batt.is_present = true;
    frame->mcb_batt.value = (uint32_t)(data.mcb.battery_voltage * 100.0f);

    frame->gps_lat.is_present = true;
    frame->gps_lat.value = data.mcb.latitude;

    frame->gps_long.is_present = true;
    frame->gps_long.value = data.mcb.longitude;

    // gps_sat in proto is a bool — set true if number of satellites > 0
    frame->gps_sat_ok.is_present = true;
    frame->gps_sat_ok.value = data.mcb.satellites_in_view > 0;

    frame->altitude_m.is_present = true;
    frame->altitude_m.value = (int32_t)data.mcb.altitude;

    frame->velocity_m_s.is_present = true;
    frame->velocity_m_s.value = (int32_t)data.mcb.velocity;

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
        recovery_flags |= data.recovery.telemetrum_armed ? 1u << 0 : 0u;
        recovery_flags |= data.recovery.telemetrum_apogee_detected ? 1u << 1 : 0u;
        recovery_flags |= data.recovery.telemetrum_first_stage ? 1u << 2 : 0u;
        recovery_flags |= data.recovery.telemetrum_second_stage ? 1u << 3 : 0u;
        recovery_flags |= data.recovery.easymini_armed ? 1u << 4 : 0u;
        recovery_flags |= data.recovery.easymini_apogee_detected ? 1u << 5 : 0u;
        recovery_flags |= data.recovery.easymini_first_stage ? 1u << 6 : 0u;
        recovery_flags |= data.recovery.easymini_second_stage ? 1u << 7 : 0u;
        recovery_flags |= data.recovery.separation_one ? 1u << 8 : 0u;
        recovery_flags |= data.recovery.separation_two ? 1u << 9 : 0u;
        recovery_flags |= data.recovery.continuity ? 1u << 10 : 0u;
        frame->recovery_flags.is_present = true;
        frame->recovery_flags.value = recovery_flags;
    }

    // pitot
    frame->pitot_battery.is_present = true;
    frame->pitot_battery.value = (uint32_t)(data.pitot.vbat * 100.0f);

    frame->pitot_altitude.is_present = true;
    frame->pitot_altitude.value = (int32_t)data.pitot.alt;

    frame->pitot_velocity.is_present = true;
    frame->pitot_velocity.value = (int32_t)data.pitot.speed;

    frame->pitot_temperature.is_present = true;
    frame->pitot_temperature.value = (int32_t)data.pitot.temperature;

    uint32_t valve_states_bitfield = 0;
    valve_states_bitfield |= data.ox_main_valve.valve_1_state == 1 ? 1u << 0 : 0u;
    valve_states_bitfield |= data.eth_vent_n2_main_valves.valve_2_state == 1 ? 1u << 1 : 0u;
    valve_states_bitfield |= data.ox_vent_eth_main_valves.valve_1_state == 1 ? 1u << 2 : 0u;
    valve_states_bitfield |= data.ox_vent_eth_main_valves.valve_2_state == 1 ? 1u << 3 : 0u;
    valve_states_bitfield |= data.eth_vent_n2_main_valves.valve_1_state == 1 ? 1u << 4 : 0u;
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
        int16_t pressure = (int16_t)(data.ox_vent_eth_main_valves.pressure_1 * 100.0f);
        v |= (uint32_t)(uint16_t)pressure << 16;
        uint8_t battery_voltage = (uint8_t)(data.ox_vent_eth_main_valves.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 8;
        uint8_t battery_consumption = (uint8_t)(data.ox_vent_eth_main_valves.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 0;
        frame->ox_vent_eth_main_bit_data_a.is_present = true;
        frame->ox_vent_eth_main_bit_data_a.value = v;
    }

    {
        uint32_t v = 0;
        v |= (uint32_t)data.ox_vent_eth_main_valves.is_charging << 31;
        int8_t charger_temperature = (int8_t)data.ox_vent_eth_main_valves.charger_temperature;
        v |= (uint32_t)(uint8_t)charger_temperature << 23;
        int16_t pressure = (int16_t)(data.ox_vent_eth_main_valves.pressure_2 * 100);
        v |= (uint32_t)(uint16_t)pressure << 7;
        frame->ox_vent_eth_main_bit_data_b.is_present = true;
        frame->ox_vent_eth_main_bit_data_b.value = v;
    }

    {
        uint32_t v = 0;
        v |= (uint32_t)data.ox_vent_eth_main_valves.auto_vent_activated << 31;
        v |= (uint32_t)data.ox_vent_eth_main_valves.auto_vent_triggered << 30;
        int8_t ox_temperature = (int8_t)data.ox_vent_eth_main_valves.ox_temperature;
        v |= (uint32_t)(uint8_t)ox_temperature << 22;
        frame->ox_vent_eth_main_bit_data_c.is_present = true;
        frame->ox_vent_eth_main_bit_data_c.value = v;
    }

    {
        frame->auto_vent_setting.is_present = true;
        frame->auto_vent_setting.value = data.ox_vent_eth_main_valves.auto_vent_pressure;
    }

    // ox main bit data (uses ox_main_valve struct)
    {
        uint32_t v = 0;
        int16_t pressure = (int16_t)(data.ox_main_valve.pressure_1 * 100.0f);
        v |= (uint32_t)(uint16_t)pressure << 16;
        v |= (uint32_t)data.ox_main_valve.dump_valve_arm << 15;
        v |= (uint32_t)data.ox_main_valve.dump_valve_cont << 14;
        int8_t temperature = (int8_t)data.ox_main_valve.temperature_1;
        v |= (uint32_t)(uint8_t)temperature << 6;
        frame->ox_main_bit_data_a.is_present = true;
        frame->ox_main_bit_data_a.value = v;
    }

    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)(data.ox_main_valve.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)(data.ox_main_valve.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.ox_main_valve.is_charging << 15;
        int8_t charger_temperature = (int8_t)data.ox_main_valve.charger_temperature;
        v |= (uint32_t)(uint8_t)charger_temperature << 7;
        frame->ox_main_bit_data_b.is_present = true;
        frame->ox_main_bit_data_b.value = v;
    }

    // n2 vent bit data (uses n2_vent_valve struct)
    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)(data.n2_vent_valve.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)(data.n2_vent_valve.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.n2_vent_valve.is_charging << 15;
        int8_t charger_temperature = (int8_t)data.n2_vent_valve.charger_temperature;
        v |= (uint32_t)(uint8_t)charger_temperature << 7;
        frame->n2_vent_bit_data_a.is_present = true;
        frame->n2_vent_bit_data_a.value = v;
    }

    // eth vent n2 main bit data (uses eth_vent_n2_main_valve struct)
    {
        uint32_t v = 0;
        uint8_t battery_voltage = (uint8_t)(data.eth_vent_n2_main_valves.battery_voltage * 10.0f);
        v |= (uint32_t)battery_voltage << 24;
        uint8_t battery_consumption = (uint8_t)(data.eth_vent_n2_main_valves.battery_consumption * 10.0f);
        v |= (uint32_t)battery_consumption << 16;
        v |= (uint32_t)data.eth_vent_n2_main_valves.is_charging << 15;
        int8_t charger_temperature = (int8_t)data.eth_vent_n2_main_valves.charger_temperature;
        v |= (uint32_t)(uint8_t)charger_temperature << 7;
        frame->eth_vent_n2_main_bit_data_a.is_present = true;
        frame->eth_vent_n2_main_bit_data_a.value = v;
    }

    {
        uint32_t v = 0;
        int16_t pressure_1 = (int16_t)(data.eth_vent_n2_main_valves.pressure_1 * 100);
        v |= (uint32_t)(uint16_t)pressure_1 << 16;
        int16_t pressure_2 = (int16_t)(data.eth_vent_n2_main_valves.pressure_2 * 100);
        v |= (uint32_t)(uint16_t)pressure_2;
        frame->eth_vent_n2_main_bit_data_b.is_present = true;
        frame->eth_vent_n2_main_bit_data_b.value = v;
    }

    // payload
    frame->payload_battery.is_present = true;
    frame->payload_battery.value = (uint32_t)(data.payload.vbat * 100.0f);

    {
        uint32_t conn = 0;
        conn |= data.connected_dev.payload ? 1u << 0 : 0u; // payload_connected
        conn |= data.connected_dev.tanwa ? 1u << 1 : 0u; // tanwa_connected
        conn |= data.connected_dev.eth_vent_n2_main_valves ? 1u << 2 : 0u;
        conn |= data.connected_dev.ox_main_valve ? 1u << 3 : 0u;
        conn |= data.connected_dev.ox_vent_eth_main_valves ? 1u << 4 : 0u;
        conn |= data.connected_dev.n2_vent_valve ? 1u << 5 : 0u;
        conn |= data.connected_dev.pitot ? 1u << 6 : 0u;
        frame->esp_now_connected_flags.is_present = true;
        frame->esp_now_connected_flags.value = conn;
    }

    {
        uint32_t wk = 0;
        wk |= data.payload.waken_up ? 1u << 0 : 0u;
        wk |= data.eth_vent_n2_main_valves.waken_up ? 1u << 1 : 0u;
        wk |= data.ox_main_valve.waken_up ? 1u << 2 : 0u;
        wk |= data.ox_vent_eth_main_valves.waken_up ? 1u << 3 : 0u;
        wk |= data.n2_vent_valve.waken_up ? 1u << 4 : 0u;
        wk |= data.pitot.waken_up ? 1u << 5 : 0u;
        frame->esp_now_wkup_flags.is_present = true;
        frame->esp_now_wkup_flags.value = wk;
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