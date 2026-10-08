#ifndef ZCL_VALUES_H
#define ZCL_VALUES_H
#include <stdbool.h>
#include <stdint.h>
#include "measurement.h"
/* ZCL Measurement & Sensing cluster attribute representations. */
#define ZCL_INVALID_MEASURED_VALUE 0xffffu
uint16_t room_lux_to_zcl_illuminance(uint32_t lux);
#endif
