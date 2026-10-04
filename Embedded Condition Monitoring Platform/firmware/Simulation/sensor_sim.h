#ifndef SENSOR_SIM_H
#define SENSOR_SIM_H
#include "../Drivers/common/sensor_backend.h"
typedef enum { SIM_VIBRATION, SIM_TEMPERATURE, SIM_CURRENT, SIM_MICROPHONE } SimSensorKind;
typedef struct { SimSensorKind kind; unsigned sample_index; unsigned seed; float sample_rate_hz; float shaft_hz; float severity; unsigned fault; int running; } SensorSim;
SensorBackend sensor_sim_backend(SensorSim *sim);
void sensor_sim_configure(SensorSim *sim, SimSensorKind kind, float sample_rate_hz, float shaft_hz, unsigned fault, float severity, unsigned seed);
#endif
