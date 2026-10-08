#include "measurement.h"
#include <math.h>
bool room_measurement_from_si(float t, float h, float lux, room_measurement_t *out) {
    if (out == 0 || !isfinite(t) || !isfinite(h) || !isfinite(lux) || t < -40.0f || t > 125.0f || h < 0.0f || h > 100.0f || lux < 0.0f || lux > 1000000.0f) return false;
    out->temperature_centi_c = (int16_t)lroundf(t * 100.0f);
    out->humidity_centi_pct = (uint16_t)lroundf(h * 100.0f);
    out->illuminance_lux = (uint32_t)lroundf(lux);
    return true;
}
