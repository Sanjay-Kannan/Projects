#include "room_policy.h"
void room_policy_init(room_policy_t *p){if(!p)return;p->fan_on_c=28.0f;p->fan_off_c=26.0f;p->light_on_lux=100.0f;p->light_off_lux=140.0f;p->fan_on=false;p->light_on=false;}
void room_policy_measure(room_policy_t *p,float t,float lux){if(!p)return;if(t>=-40.0f&&t<=125.0f){if(t>=p->fan_on_c)p->fan_on=true;else if(t<=p->fan_off_c)p->fan_on=false;}if(lux>=0.0f&&lux<=100000.0f){if(lux<p->light_on_lux)p->light_on=true;else if(lux>p->light_off_lux)p->light_on=false;}}
