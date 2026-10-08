#include "device_control.h"
#include <string.h>
static void emit(device_control_t *c){if(c&&c->output)c->output(&c->state,c->context);}
void device_control_init(device_control_t *c,control_output_fn fn,void *ctx){if(!c)return;memset(c,0,sizeof(*c));c->output=fn;c->context=ctx;emit(c);}
void device_control_onoff(device_control_t *c,bool on){if(!c)return;room_light_set(&c->state,on);emit(c);}
void device_control_level(device_control_t *c,uint8_t level){if(!c)return;room_light_level(&c->state,level);emit(c);}
void device_control_fan(device_control_t *c,bool on,uint8_t pct){if(!c)return;c->state.on=on;room_fan_percent(&c->state,on?pct:0u);emit(c);}
