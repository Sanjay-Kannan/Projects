#include "temperature_sensor.h"
#include <math.h>
SensorStatus temperature_sensor_init(TemperatureSensor*s,SensorBackend b){if(!s||!b.read||!b.start||!b.stop)return SENSOR_INVALID_ARGUMENT;s->backend=b;s->minimum_c=-40;s->maximum_c=125;s->state=SENSOR_STATE_READY;return SENSOR_OK;}
SensorStatus temperature_sensor_configure(TemperatureSensor*s,unsigned rate){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;if(rate==0||rate>100)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorStatus temperature_sensor_start(TemperatureSensor*s){if(!s)return SENSOR_INVALID_ARGUMENT;if(s->state==SENSOR_STATE_OFF)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.start(s->backend.context);s->state=rc==SENSOR_OK?SENSOR_STATE_RUNNING:SENSOR_STATE_ERROR;return rc;}
SensorStatus temperature_sensor_read(TemperatureSensor*s,float*out){if(!s||!out)return SENSOR_INVALID_ARGUMENT;if(s->state!=SENSOR_STATE_RUNNING)return SENSOR_NOT_INITIALIZED;SensorStatus rc=s->backend.read(s->backend.context,out,1);if(rc!=SENSOR_OK){s->state=SENSOR_STATE_ERROR;return rc;}if(!isfinite(*out)||*out<s->minimum_c||*out>s->maximum_c)return SENSOR_OUT_OF_RANGE;return SENSOR_OK;}
SensorState temperature_sensor_get_status(const TemperatureSensor*s){return s?s->state:SENSOR_STATE_ERROR;}
void temperature_sensor_shutdown(TemperatureSensor*s){if(s&&s->state!=SENSOR_STATE_OFF){s->backend.stop(s->backend.context);s->state=SENSOR_STATE_OFF;}}
