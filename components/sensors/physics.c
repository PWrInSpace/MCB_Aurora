#include "physics.h"

#include <tgmath.h>

float hypsometric_altitude(float pressure_hpa, float pressure0_hpa, float temperature_c) {
    if (!(pressure_hpa > 0.0f) || !(pressure0_hpa > 0.0f)) {
        return 0.0f;
    }

    float temperature_k = temperature_c + 273.15f;
    float ratio = pressure0_hpa / pressure_hpa;

    return 29.271f * temperature_k * logf(ratio);
}