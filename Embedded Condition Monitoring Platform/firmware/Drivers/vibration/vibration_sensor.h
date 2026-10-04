#ifndef VIBRATION_SENSOR_H
#define VIBRATION_SENSOR_H
#include "sensor_backend.h"
typedef struct { SensorBackend backend; SensorState state; float full_scale_g; } VibrationSensor;
SensorStatus vibration_sensor_init(VibrationSensor *, SensorBackend, float full_scale_g);
SensorStatus vibration_sensor_configure(VibrationSensor *, unsigned sample_rate_hz);
SensorStatus vibration_sensor_start(VibrationSensor *);
SensorStatus vibration_sensor_read(VibrationSensor *, float xyz_g[3]);
SensorState vibration_sensor_get_status(const VibrationSensor *);
void vibration_sensor_shutdown(VibrationSensor *);
#endif
