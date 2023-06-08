// Copyright 2022 PWrInSpace, Krzysztof Gliwiński

#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "soc/adc_channel.h"

/*!
  \file voltageMeasure.h contains structs and functions to handle
        ESP32's ADC1 only!
*/

#define READ_ERROR_RETURN_VAL 0xFFFF
#define VOLTAGE_READ_ERROR_RETURN_VAL -1.0f

typedef enum { adcOK = 0, adcConfigError} AdcResult;

typedef struct {
  float adcCal;
  uint8_t adcChannel;
} VoltageMeasure;

/*! 
  \brief Init for a voltage measure.
  \param vMes - pointer to VoltageMeasure struct
  \param adcChannel - specific channel of ADC1
  \param adcCal - calibration value to be configured.
                  voltage = rawRead * adcCal
*/
AdcResult voltageMeasureInit(VoltageMeasure* vMes, uint8_t adcChannel, float adcCal);

int voltageMeasureReadRaw(VoltageMeasure* vMes);

float voltageMeasureReadVoltage(VoltageMeasure *vMes);
