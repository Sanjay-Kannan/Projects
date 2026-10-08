#ifndef MEASUREMENT_H
#define MEASUREMENT_H
#include <stdbool.h>
#include <stdint.h>
/* ZCL Measurement & Sensing values use hundredths of a degree / percent. */
typedef struct { int16_t temperature_centi_c; uint16_t humidity_centi_pct; uint32_t illuminance_lux; } room_measurement_t;
bool room_measurement_from_si(float temperature_c, float humidity_pct, float lux, room_measurement_t *out);
#endif
