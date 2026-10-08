#ifndef ACTUATOR_H
#define ACTUATOR_H
#include <stdbool.h>
#include <stdint.h>
typedef struct { bool on; uint8_t level; uint8_t fan_percent; } room_actuator_t;
void room_light_set(room_actuator_t *state, bool on);
void room_light_level(room_actuator_t *state, uint8_t level);
void room_fan_percent(room_actuator_t *state, uint8_t percent);
#endif
