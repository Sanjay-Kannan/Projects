#include "veml6030.h"
#define VEML_REG_ALS_CONF 0x00u
#define VEML_REG_ALS_DATA 0x04u
static HAL_StatusTypeDef write_reg(I2C_HandleTypeDef *bus,uint16_t address,uint8_t reg,uint16_t value) {
    uint8_t b[3]={reg,(uint8_t)value,(uint8_t)(value>>8)}; return HAL_I2C_Master_Transmit(bus,address,b,3u,100u);
}
static HAL_StatusTypeDef read_reg(I2C_HandleTypeDef *bus,uint16_t address,uint8_t reg,uint16_t *value) {
    uint8_t b[2]; HAL_StatusTypeDef s=HAL_I2C_Master_Transmit(bus,address,&reg,1u,100u); if(s!=HAL_OK)return s;
    s=HAL_I2C_Master_Receive(bus,address,b,2u,100u); if(s==HAL_OK)*value=(uint16_t)(b[0]|((uint16_t)b[1]<<8)); return s;
}
HAL_StatusTypeDef veml6030_init(veml6030_t *s,I2C_HandleTypeDef *bus,uint16_t address) {
    if (s == NULL || bus == NULL) return HAL_ERROR;
    s->bus = bus; s->address = address; s->resolution_lux_per_count = 0.0576f;
    /* ALS gain 1/4, integration time 100 ms, power on. */
    return write_reg(bus,address,VEML_REG_ALS_CONF,0x1000u);
}
HAL_StatusTypeDef veml6030_read_lux(veml6030_t *s,float *lux) {
    uint16_t raw; if(!s||!s->bus||!lux)return HAL_ERROR;
    HAL_StatusTypeDef status=read_reg(s->bus,s->address,VEML_REG_ALS_DATA,&raw);
    if(status==HAL_OK)*lux=(float)raw*s->resolution_lux_per_count;
    return status;
}
