#ifndef VEML6030_H
#define VEML6030_H
#include "stm32wbxx_hal.h"
typedef struct { I2C_HandleTypeDef *bus; uint16_t address; float resolution_lux_per_count; } veml6030_t;
HAL_StatusTypeDef veml6030_init(veml6030_t *sensor, I2C_HandleTypeDef *bus, uint16_t address);
HAL_StatusTypeDef veml6030_read_lux(veml6030_t *sensor, float *lux);
#endif
