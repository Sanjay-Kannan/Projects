#include "sht31.h"
static uint8_t crc8(const uint8_t *data, unsigned length) {
    uint8_t crc = 0xffu;
    for (unsigned i=0; i<length; ++i) { crc ^= data[i]; for (unsigned b=0; b<8u; ++b) crc = (crc & 0x80u) ? (uint8_t)((crc << 1) ^ 0x31u) : (uint8_t)(crc << 1); }
    return crc;
}
HAL_StatusTypeDef sht31_init(sht31_t *s, I2C_HandleTypeDef *bus, uint16_t address) {
    if (s == NULL || bus == NULL) return HAL_ERROR;
    s->bus = bus;
    s->address = address;
    return HAL_OK;
}
HAL_StatusTypeDef sht31_read(sht31_t *s, float *t, float *h) {
    uint8_t cmd[2]={0x24u,0x00u}, raw[6];
    if (!s || !s->bus || !t || !h) return HAL_ERROR;
    HAL_StatusTypeDef status=HAL_I2C_Master_Transmit(s->bus,s->address,cmd,sizeof(cmd),100u);
    if(status!=HAL_OK) return status;
    HAL_Delay(20u);
    status=HAL_I2C_Master_Receive(s->bus,s->address,raw,sizeof(raw),100u);
    if(status!=HAL_OK) return status;
    if(crc8(raw,2u)!=raw[2] || crc8(&raw[3],2u)!=raw[5]) return HAL_ERROR;
    uint16_t tr=(uint16_t)(((uint16_t)raw[0]<<8)|raw[1]);
    uint16_t hr=(uint16_t)(((uint16_t)raw[3]<<8)|raw[4]);
    *t=-45.0f+175.0f*((float)tr/65535.0f);
    *h=100.0f*((float)hr/65535.0f);
    return HAL_OK;
}
