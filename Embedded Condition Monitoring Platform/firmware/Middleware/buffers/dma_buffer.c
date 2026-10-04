#include "dma_buffer.h"
#include <math.h>
#include <string.h>
void dma_buffer_init(DmaBuffer*b){if(b)memset(b,0,sizeof(*b));}
float*dma_buffer_active(DmaBuffer*b){return b?b->blocks[b->active].samples:0;}
void dma_buffer_complete_isr(DmaBuffer*b,size_t count,uint64_t ts){if(!b)return;unsigned done=b->active;if(count>DMA_BLOCK_CAPACITY){count=DMA_BLOCK_CAPACITY;b->invalid_samples++;}DmaBlock*d=&b->blocks[done];for(size_t i=0;i<count;i++)if(!isfinite(d->samples[i]))b->invalid_samples++;d->count=count;d->sequence=b->next_sequence++;d->timestamp_us=ts;if((b->ready_mask|b->busy_mask)&(1u<<done)){b->overflows++;b->dropped_samples+=(uint32_t)count;}else b->ready_mask|=1u<<done;b->active^=1u;}
int dma_buffer_take(DmaBuffer*b,DmaBlock**out){if(!b||!out)return -1;for(unsigned i=0;i<2;i++)if(b->ready_mask&(1u<<i)){b->ready_mask&=~(1u<<i);b->busy_mask|=1u<<i;*out=&b->blocks[i];return 0;}return 1;}
void dma_buffer_release(DmaBuffer*b,DmaBlock*d){if(!b||!d)return;for(unsigned i=0;i<2;i++)if(d==&b->blocks[i]){b->busy_mask&=~(1u<<i);return;}}
