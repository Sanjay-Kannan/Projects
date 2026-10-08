#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H
#include "actuator.h"
#include <stdbool.h>
#include <stdint.h>
typedef void (*control_output_fn)(const room_actuator_t *state,void *context);
typedef struct { room_actuator_t state; control_output_fn output; void *context; } device_control_t;
void device_control_init(device_control_t *control,control_output_fn output,void *context);
void device_control_onoff(device_control_t *control,bool on);
void device_control_level(device_control_t *control,uint8_t level);
void device_control_fan(device_control_t *control,bool on,uint8_t percent);
#endif
