#include "dma_buffer.h"
#include "filters.h"
#include "protocol.h"
#include "dsp.h"
#include <assert.h>
#include <math.h>
#include <string.h>
int main(void){
 DmaBuffer b;dma_buffer_init(&b);DmaBlock *none=0;assert(dma_buffer_take(&b,&none)==1);
 float *a=dma_buffer_active(&b);for(unsigned i=0;i<DMA_BLOCK_CAPACITY;i++)a[i]=(float)i;
 dma_buffer_complete_isr(&b,DMA_BLOCK_CAPACITY,100);DmaBlock *first=0;assert(dma_buffer_take(&b,&first)==0);assert(first->sequence==0&&first->timestamp_us==100&&first->samples[10]==10);
 float *next=dma_buffer_active(&b);for(unsigned i=0;i<DMA_BLOCK_CAPACITY;i++)next[i]=1;
 dma_buffer_complete_isr(&b,DMA_BLOCK_CAPACITY,200);DmaBlock *second=0;assert(dma_buffer_take(&b,&second)==0&&second->sequence==1);dma_buffer_release(&b,first);dma_buffer_release(&b,second);assert(dma_buffer_take(&b,&first)==1);
 dma_buffer_complete_isr(&b,DMA_BLOCK_CAPACITY,300);dma_buffer_complete_isr(&b,DMA_BLOCK_CAPACITY,400);dma_buffer_complete_isr(&b,DMA_BLOCK_CAPACITY,500);assert(b.overflows==1&&b.dropped_samples==DMA_BLOCK_CAPACITY);
 DcBlocker hp;dc_blocker_init(&hp,1.0f,1000.0f);float y=dc_blocker_process(&hp,1.0f);assert(y>0.9f&&y<1.0f);for(int i=0;i<10000;i++)y=dc_blocker_process(&hp,1.0f);assert(fabsf(y)<0.02f);
 uint8_t frame[FRAME_SIZE];assert(protocol_encode(frame,sizeof(frame),77,1234,.5f,42.0f,1.2f,2,1)==FRAME_SIZE);uint32_t seq=0;float rms=0;assert(protocol_decode(frame,sizeof(frame),&seq,&rms)==0&&seq==77&&fabsf(rms-.5f)<1e-6f);assert(protocol_decode(frame,sizeof(frame)-1,&seq,&rms)==-1);frame[13]^=1;assert(protocol_decode(frame,sizeof(frame),&seq,&rms)==-2);
 float sine[2048];for(unsigned i=0;i<2048;i++)sine[i]=sinf(2.0f*3.14159265f*30.0f*i/10000.0f);DspFeatures f;dsp_process_configured(sine,2048,10000,1.0f,&f);assert(fabsf(f.dominant_hz-30.0f)<8.0f);assert(f.rms>.6f&&f.rms<.8f);return 0;
}
