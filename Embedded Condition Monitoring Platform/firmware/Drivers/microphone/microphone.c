#include "microphone.h"
#include <math.h>
SensorStatus microphone_init(Microphone*s,SensorBackend b,float scale){if(!s||!b.read||!b.start||!b.stop||scale<=0)return SENSOR_INVALID_ARGUMENT;s->backend=b;s->full_scale=scale;s->state=SENSOR_STATE_READY;return SENSOR_OK;}
SensorStatus microphone_configure(Microphone*s,unsigned rate){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;if(rate<1000||rate>48000)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorStatus microphone_start(Microphone*s){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.start(s->backend.context);s->state=rc==SENSOR_OK?SENSOR_STATE_RUNNING:SENSOR_STATE_ERROR;return rc;}
SensorStatus microphone_read(Microphone*s,float*out){if(!s||!out)return SENSOR_INVALID_ARGUMENT;if(s->state!=SENSOR_STATE_RUNNING)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.read(s->backend.context,out,1);if(rc!=SENSOR_OK){s->state=SENSOR_STATE_ERROR;return rc;}if(!isfinite(*out)||fabsf(*out)>s->full_scale)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorState microphone_get_status(const Microphone*s){return s?s->state:SENSOR_STATE_ERROR;}
void microphone_shutdown(Microphone*s){if(s&&s->state!=SENSOR_STATE_OFF){s->backend.stop(s->backend.context);s->state=SENSOR_STATE_OFF;}}
