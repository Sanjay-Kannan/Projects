#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stddef.h>
#include <stdint.h>
#define FRAME_SIZE 25u
uint16_t protocol_crc16(const uint8_t *data,size_t n);
size_t protocol_encode(uint8_t *out,size_t cap,uint32_t sequence,uint32_t timestamp_ms,float rms,float temp,float current,uint8_t fault,uint8_t health);
int protocol_decode(const uint8_t *in,size_t n,uint32_t *sequence,float *rms);
#endif
