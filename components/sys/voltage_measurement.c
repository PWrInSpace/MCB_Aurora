// Copyright 2022 PWrInSpace, Krzysztof Gliwiński

#include "voltage_measurement.h"

#define ONESHOT_UNIT_UNDEFINED 0
#define ONESHOT_UNIT_DEFINED 1

/// \brief Standard config, enables ADC on the whole 0 - 3.3V
/// range on ESP32 ADC1
static adc_oneshot_chan_cfg_t configAdc1 = {.bitwidth = ADC_BITWIDTH_DEFAULT,
                                            .atten = ADC_ATTEN_DB_11};

static adc_oneshot_unit_init_cfg_t initConfigAdc1 = {
    .unit_id = ADC_UNIT_1,
};

static adc_oneshot_unit_handle_t handleAdc1;

AdcResult voltageMeasureInit(VoltageMeasure* vMes, uint8_t adcChannel,
                             float adcCal) {
  esp_err_t res = ESP_OK;
  static bool oneshotUnitInitiated = false;
  vMes->adcCal = adcCal;
  vMes->adcChannel = adcChannel;
  if (oneshotUnitInitiated == ONESHOT_UNIT_UNDEFINED) {
    res |= adc_oneshot_new_unit(&initConfigAdc1, &handleAdc1);
    oneshotUnitInitiated = ONESHOT_UNIT_DEFINED;
  }
  res |= adc_oneshot_config_channel(handleAdc1, adcChannel, &configAdc1);

  return res == ESP_OK ? adcOK : adcConfigError;
}

int voltageMeasureReadRaw(VoltageMeasure* vMes) {
  int vRaw;
  if (adc_oneshot_read(handleAdc1, vMes->adcChannel, &vRaw) == ESP_FAIL) {
    return READ_ERROR_RETURN_VAL;
  }
  return vRaw;
}

float voltageMeasureReadVoltage(VoltageMeasure* vMes) {
  int vRaw = voltageMeasureReadRaw(vMes);
  if (vRaw == READ_ERROR_RETURN_VAL) {
    return VOLTAGE_READ_ERROR_RETURN_VAL;
  }

  return vMes->adcCal * (float)vRaw / 4095.0f * 4.76 - 0.82;
}
