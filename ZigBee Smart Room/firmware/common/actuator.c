#include "actuator.h"
void room_light_set(room_actuator_t *s, bool on) { if (s != 0) { s->on = on; if (!on) s->level = 0u; else if (s->level == 0u) s->level = 254u; } }
void room_light_level(room_actuator_t *s, uint8_t level) { if (s != 0) { s->level = level; s->on = level != 0u; } }
void room_fan_percent(room_actuator_t *s, uint8_t percent) { if (s != 0) s->fan_percent = percent > 100u ? 100u : percent; }
