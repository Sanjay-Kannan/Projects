#ifndef ROOM_POLICY_H
#define ROOM_POLICY_H
#include <stdbool.h>
typedef struct { float fan_on_c,fan_off_c,light_on_lux,light_off_lux; bool fan_on,light_on; } room_policy_t;
void room_policy_init(room_policy_t *p);
void room_policy_measure(room_policy_t *p,float temperature_c,float illuminance_lux);
#endif
