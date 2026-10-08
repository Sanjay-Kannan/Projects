#include "env_app.h"
#include "sht31.h"
#include "veml6030.h"
#include "zcl_values.h"
#include <string.h>
static sht31_t sht; static veml6030_t veml; static uint32_t last_sample;
void env_app_init(env_app_t *app,I2C_HandleTypeDef *i2c,env_report_fn report,void *ctx) {
    if(!app)return; memset(app,0,sizeof(*app)); app->i2c=i2c;app->sht31_address=(uint16_t)(0x44u<<1);app->veml6030_address=(uint16_t)(0x10u<<1);app->sample_period_ms=5000u;app->report=report;app->context=ctx;
    (void)sht31_init(&sht,i2c,app->sht31_address);(void)veml6030_init(&veml,i2c,app->veml6030_address);last_sample=HAL_GetTick();
}
void env_app_poll(env_app_t *app,uint32_t now) {
    float t,h,lux; room_measurement_t m;
    if(!app||!app->report||(uint32_t)(now-last_sample)<app->sample_period_ms)return;
    last_sample=now;
    if(sht31_read(&sht,&t,&h)!=HAL_OK||veml6030_read_lux(&veml,&lux)!=HAL_OK)return;
    if(!room_measurement_from_si(t,h,lux,&m))return;
    /* Keep raw SI lux alongside the proper ZCL encoded value at the ZCL adapter boundary. */
    (void)room_lux_to_zcl_illuminance((uint32_t)lux);
    app->report(&m,app->context);
}
