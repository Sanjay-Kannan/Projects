#include "vibration_sensor.h"
#include <math.h>
SensorStatus vibration_sensor_init(VibrationSensor *s, SensorBackend b, float range) {
    if (!s || !b.read || !b.start || !b.stop || range <= 0.0f) return SENSOR_INVALID_ARGUMENT;
    s->backend=b; s->full_scale_g=range; s->state=SENSOR_STATE_READY; return SENSOR_OK;
}
SensorStatus vibration_sensor_configure(VibrationSensor *s, unsigned rate) {
    if (!s) return SENSOR_INVALID_ARGUMENT;
    if (s->state==SENSOR_STATE_OFF) return SENSOR_NOT_INITIALIZED;
    if (rate<100 || rate>32000) return SENSOR_OUT_OF_RANGE;
    return SENSOR_OK;
}
SensorStatus vibration_sensor_start(VibrationSensor *s) {
    if (!s) return SENSOR_INVALID_ARGUMENT;
    if (s->state==SENSOR_STATE_OFF) return SENSOR_NOT_INITIALIZED;
    SensorStatus rc=s->backend.start(s->backend.context); s->state=rc==SENSOR_OK?SENSOR_STATE_RUNNING:SENSOR_STATE_ERROR; return rc;
}
SensorStatus vibration_sensor_read(VibrationSensor *s,float xyz[3]) {
    if (!s || !xyz) return SENSOR_INVALID_ARGUMENT;
    if (s->state!=SENSOR_STATE_RUNNING) return SENSOR_NOT_INITIALIZED;
    SensorStatus rc=s->backend.read(s->backend.context,xyz,3);
    if (rc!=SENSOR_OK) { s->state=SENSOR_STATE_ERROR; return rc; }
    for(unsigned i=0;i<3;i++) if(!isfinite(xyz[i]) || fabsf(xyz[i])>s->full_scale_g) return SENSOR_OUT_OF_RANGE;
    return SENSOR_OK;
}
SensorState vibration_sensor_get_status(const VibrationSensor *s){return s?s->state:SENSOR_STATE_ERROR;}
void vibration_sensor_shutdown(VibrationSensor *s){if(s&&s->state!=SENSOR_STATE_OFF){s->backend.stop(s->backend.context);s->state=SENSOR_STATE_OFF;}}
