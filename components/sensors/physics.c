#include "physics.h"

#include <tgmath.h>

float hypsometric_altitude(float pressure_hpa, float pressure0_hpa, float temperature_c) {
    if (!(pressure_hpa > 0.0) || !(pressure0_hpa > 0.0)) {
        return 0.0;
    }

    float temperature_k = temperature_c + 273.15;
    float ratio = pressure_hpa / pressure0_hpa;

    return 29.271 * temperature_k * log(ratio);
}