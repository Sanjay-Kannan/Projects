#ifndef MICROPHONE_H
#define MICROPHONE_H
#include "sensor_backend.h"
typedef struct { SensorBackend backend; SensorState state; float full_scale; } Microphone;
SensorStatus microphone_init(Microphone *, SensorBackend, float full_scale);
SensorStatus microphone_configure(Microphone *, unsigned sample_rate_hz);
SensorStatus microphone_start(Microphone *);
SensorStatus microphone_read(Microphone *, float *normalized_sample);
SensorState microphone_get_status(const Microphone *);
void microphone_shutdown(Microphone *);
#endif
