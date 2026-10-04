#include "current_sensor.h"
#include <math.h>
SensorStatus current_sensor_init(CurrentSensor*s,SensorBackend b,float max){if(!s||!b.read||!b.start||!b.stop||max<=0)return SENSOR_INVALID_ARGUMENT;s->backend=b;s->maximum_a=max;s->state=SENSOR_STATE_READY;return SENSOR_OK;}
SensorStatus current_sensor_configure(CurrentSensor*s,unsigned rate){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;if(rate==0||rate>20000)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorStatus current_sensor_start(CurrentSensor*s){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.start(s->backend.context);s->state=rc==SENSOR_OK?SENSOR_STATE_RUNNING:SENSOR_STATE_ERROR;return rc;}
SensorStatus current_sensor_read(CurrentSensor*s,float*out){if(!s||!out)return SENSOR_INVALID_ARGUMENT;if(s->state!=SENSOR_STATE_RUNNING)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.read(s->backend.context,out,1);if(rc!=SENSOR_OK){s->state=SENSOR_STATE_ERROR;return rc;}if(!isfinite(*out)||*out<0||*out>s->maximum_a)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorState current_sensor_get_status(const CurrentSensor*s){return s?s->state:SENSOR_STATE_ERROR;}
void current_sensor_shutdown(CurrentSensor*s){if(s&&s->state!=SENSOR_STATE_OFF){s->backend.stop(s->backend.context);s->state=SENSOR_STATE_OFF;}}
