#ifndef SHT31_H
#define SHT31_H
#include "stm32wbxx_hal.h"
#include <stdbool.h>
typedef struct { I2C_HandleTypeDef *bus; uint16_t address; } sht31_t;
HAL_StatusTypeDef sht31_init(sht31_t *sensor, I2C_HandleTypeDef *bus, uint16_t address);
HAL_StatusTypeDef sht31_read(sht31_t *sensor, float *temperature_c, float *humidity_pct);
#endif
