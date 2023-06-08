#ifndef VBAT_WRAPPER
#define VBAT_WRAPPER
#include "voltage_measurement.h"

static VoltageMeasure vmeas;

bool vbat_init(void) {
    if (voltageMeasureInit(&vmeas, ADC_CHANNEL_3, 3.3f) != adcOK) {
        return false;
    }

    return true;
}

float vbat_read(void) {
    return voltageMeasureReadVoltage(&vmeas);
}

#endif