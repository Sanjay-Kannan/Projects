#include "sensor_sim.h"
#include "vibration_sensor.h"
#include "temperature_sensor.h"
#include "current_sensor.h"
#include "microphone.h"
#include <assert.h>

int main(void) {
    SensorSim sim[4];
    sensor_sim_configure(&sim[0],SIM_VIBRATION,10000,30,0,1,1);
    VibrationSensor vib;
    assert(vibration_sensor_init(&vib,sensor_sim_backend(&sim[0]),8.0f)==SENSOR_OK);
    assert(vibration_sensor_read(&vib,(float[3]){0})==SENSOR_NOT_INITIALIZED);
    assert(vibration_sensor_configure(&vib,0)==SENSOR_OUT_OF_RANGE);
    assert(vibration_sensor_configure(&vib,10000)==SENSOR_OK);
    assert(vibration_sensor_start(&vib)==SENSOR_OK);
    float xyz[3];assert(vibration_sensor_read(&vib,xyz)==SENSOR_OK);
    vibration_sensor_shutdown(&vib);assert(vibration_sensor_get_status(&vib)==SENSOR_STATE_OFF);

    sensor_sim_configure(&sim[1],SIM_TEMPERATURE,10,30,0,1,2);
    TemperatureSensor temp;assert(temperature_sensor_init(&temp,sensor_sim_backend(&sim[1]))==SENSOR_OK);
    assert(temperature_sensor_start(&temp)==SENSOR_OK);float c;
    assert(temperature_sensor_read(&temp,&c)==SENSOR_OK);assert(c>0&&c<100);temperature_sensor_shutdown(&temp);

    sensor_sim_configure(&sim[2],SIM_CURRENT,2000,30,0,1,3);
    CurrentSensor current;assert(current_sensor_init(&current,sensor_sim_backend(&sim[2]),20)==SENSOR_OK);
    assert(current_sensor_start(&current)==SENSOR_OK);float a;
    assert(current_sensor_read(&current,&a)==SENSOR_OK);assert(a>0);current_sensor_shutdown(&current);

    sensor_sim_configure(&sim[3],SIM_MICROPHONE,12000,30,0,1,4);
    Microphone mic;assert(microphone_init(&mic,sensor_sim_backend(&sim[3]),1)==SENSOR_OK);
    assert(microphone_start(&mic)==SENSOR_OK);float sample;
    assert(microphone_read(&mic,&sample)==SENSOR_OK);microphone_shutdown(&mic);
    return 0;
}
