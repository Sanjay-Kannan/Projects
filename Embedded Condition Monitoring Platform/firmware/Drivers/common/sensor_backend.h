#ifndef SENSOR_BACKEND_H
#define SENSOR_BACKEND_H

#include "sensor_status.h"

/* HAL adapters provide these operations; the application only sees drivers. */
typedef struct {
    void *context;
    SensorStatus (*start)(void *context);
    SensorStatus (*read)(void *context, float *values, unsigned count);
    void (*stop)(void *context);
} SensorBackend;

#endif
