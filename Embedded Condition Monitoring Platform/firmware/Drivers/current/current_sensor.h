#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H
#include "sensor_backend.h"
typedef struct { SensorBackend backend; SensorState state; float maximum_a; } CurrentSensor;
SensorStatus current_sensor_init(CurrentSensor *, SensorBackend, float maximum_a);
SensorStatus current_sensor_configure(CurrentSensor *, unsigned sample_rate_hz);
SensorStatus current_sensor_start(CurrentSensor *);
SensorStatus current_sensor_read(CurrentSensor *, float *current_a);
SensorState current_sensor_get_status(const CurrentSensor *);
void current_sensor_shutdown(CurrentSensor *);
#endif
