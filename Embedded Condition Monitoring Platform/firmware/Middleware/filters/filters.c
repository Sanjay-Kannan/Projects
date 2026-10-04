#include "filters.h"
#include <math.h>
void dc_blocker_init(DcBlocker*f,float cutoff,float fs){if(!f)return;f->previous_input=0;f->previous_output=0;f->highpass_hz=cutoff;f->sample_rate_hz=fs;}
float dc_blocker_process(DcBlocker*f,float x){if(!f||f->sample_rate_hz<=0||f->highpass_hz<=0)return x;float rc=1.0f/(6.28318530718f*f->highpass_hz);float dt=1.0f/f->sample_rate_hz;float a=rc/(rc+dt);float y=a*(f->previous_output+x-f->previous_input);f->previous_input=x;f->previous_output=y;return y;}
