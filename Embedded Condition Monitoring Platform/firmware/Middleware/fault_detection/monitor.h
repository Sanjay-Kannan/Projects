#ifndef MONITOR_H
#define MONITOR_H
#include "dsp.h"
typedef enum { FAULT_NORMAL, FAULT_IMBALANCE, FAULT_MISALIGNMENT, FAULT_BEARING_FAULT, FAULT_OVERLOAD } Fault;
typedef enum { HEALTH_NORMAL, HEALTH_WARNING, HEALTH_CRITICAL } Health;
typedef struct { Fault fault; Health health; float severity; char reason[160]; } MonitorResult;
MonitorResult monitor_classify(const DspFeatures *v,float current_rms,float temperature_c,float acoustic_rms,float shaft_hz);
const char *fault_name(Fault f);
#endif
