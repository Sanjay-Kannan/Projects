#ifndef ENV_APP_H
#define ENV_APP_H
#include "measurement.h"
#include "stm32wbxx_hal.h"
typedef void (*env_report_fn)(const room_measurement_t *value, void *context);
typedef struct { I2C_HandleTypeDef *i2c; uint16_t sht31_address, veml6030_address; uint32_t sample_period_ms; env_report_fn report; void *context; } env_app_t;
void env_app_init(env_app_t *app, I2C_HandleTypeDef *i2c, env_report_fn report, void *context);
void env_app_poll(env_app_t *app, uint32_t now_ms);
#endif
