#ifndef DMA_BUFFER_H
#define DMA_BUFFER_H
#include <stddef.h>
#include <stdint.h>
#define DMA_BLOCK_CAPACITY 2048u
typedef struct { float samples[DMA_BLOCK_CAPACITY]; size_t count; uint32_t sequence; uint64_t timestamp_us; } DmaBlock;
typedef struct { DmaBlock blocks[2]; volatile unsigned ready_mask; volatile unsigned busy_mask; unsigned active; uint32_t next_sequence; uint32_t overflows; uint32_t dropped_samples; uint32_t invalid_samples; } DmaBuffer;
void dma_buffer_init(DmaBuffer *buffer);
float *dma_buffer_active(DmaBuffer *buffer);
void dma_buffer_complete_isr(DmaBuffer *buffer, size_t count, uint64_t timestamp_us);
int dma_buffer_take(DmaBuffer *buffer, DmaBlock **block);
void dma_buffer_release(DmaBuffer *buffer, DmaBlock *block);
#endif
