#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H
#include "sensor_backend.h"
typedef struct { SensorBackend backend; SensorState state; float minimum_c, maximum_c; } TemperatureSensor;
SensorStatus temperature_sensor_init(TemperatureSensor *, SensorBackend);
SensorStatus temperature_sensor_configure(TemperatureSensor *, unsigned sample_rate_hz);
SensorStatus temperature_sensor_start(TemperatureSensor *);
SensorStatus temperature_sensor_read(TemperatureSensor *, float *temperature_c);
SensorState temperature_sensor_get_status(const TemperatureSensor *);
void temperature_sensor_shutdown(TemperatureSensor *);
#endif
