#include "app_config.h"
#include "dsp.h"
#include "monitor.h"
#include "protocol.h"
#include "vibration_sensor.h"
#include "temperature_sensor.h"
#include "current_sensor.h"
#include "microphone.h"
#include "sensor_sim.h"
#include "dma_buffer.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static unsigned fault_from_name(const char *name) {
    static const char *names[]={"NORMAL","IMBALANCE","MISALIGNMENT","BEARING_FAULT","OVERLOAD"};
    for(unsigned i=0;i<5;i++) if(strcmp(name,names[i])==0) return i;
    return 0;
}

int main(int argc,char **argv) {
    const char *scenario=argc>1?argv[1]:"NORMAL";
    const unsigned fault=fault_from_name(scenario), n=VIBRATION_FFT_SIZE;
    const float shaft_hz=MOTOR_RPM/60.0f;
    float *samples=malloc(n*sizeof(*samples));
    if(!samples){fprintf(stderr,"ERROR: sample allocation failed\n");return 2;}

    SensorSim vib_sim,temp_sim,current_sim,mic_sim;
    sensor_sim_configure(&vib_sim,SIM_VIBRATION,(float)VIBRATION_FS_HZ,shaft_hz,fault,1.0f,12345u);
    sensor_sim_configure(&temp_sim,SIM_TEMPERATURE,(float)TEMPERATURE_FS_HZ,shaft_hz,fault,1.0f,12346u);
    sensor_sim_configure(&current_sim,SIM_CURRENT,(float)CURRENT_FS_HZ,shaft_hz,fault,1.0f,12347u);
    sensor_sim_configure(&mic_sim,SIM_MICROPHONE,(float)MICROPHONE_FS_HZ,shaft_hz,fault,1.0f,12348u);
    VibrationSensor vibration; TemperatureSensor temperature; CurrentSensor current; Microphone microphone;
    if(vibration_sensor_init(&vibration,sensor_sim_backend(&vib_sim),8.0f)!=SENSOR_OK ||
       temperature_sensor_init(&temperature,sensor_sim_backend(&temp_sim))!=SENSOR_OK ||
       current_sensor_init(&current,sensor_sim_backend(&current_sim),20.0f)!=SENSOR_OK ||
       microphone_init(&microphone,sensor_sim_backend(&mic_sim),1.0f)!=SENSOR_OK) {
        fprintf(stderr,"ERROR: sensor initialization failed\n");free(samples);return 3;
    }
    if(vibration_sensor_configure(&vibration,VIBRATION_FS_HZ)!=SENSOR_OK ||
       temperature_sensor_configure(&temperature,TEMPERATURE_FS_HZ)!=SENSOR_OK ||
       current_sensor_configure(&current,CURRENT_FS_HZ)!=SENSOR_OK ||
       microphone_configure(&microphone,MICROPHONE_FS_HZ)!=SENSOR_OK ||
       vibration_sensor_start(&vibration)!=SENSOR_OK || temperature_sensor_start(&temperature)!=SENSOR_OK ||
       current_sensor_start(&current)!=SENSOR_OK || microphone_start(&microphone)!=SENSOR_OK) {
        fprintf(stderr,"ERROR: sensor configuration/start failed\n");free(samples);return 4;
    }
    DmaBuffer dma;dma_buffer_init(&dma);DmaBlock *held=NULL,*ready=NULL;unsigned peak_slots=0;clock_t pipeline_start=clock();
    for(unsigned block=0;block<2;block++){
        float *target=dma_buffer_active(&dma);
        for(unsigned i=0;i<n;i++){float xyz[3];if(vibration_sensor_read(&vibration,xyz)!=SENSOR_OK){fprintf(stderr,"ERROR: vibration sample invalid at %u\n",i);free(samples);return 5;}target[i]=xyz[0];}
        dma_buffer_complete_isr(&dma,n,(uint64_t)block*(uint64_t)(n*1000000u/VIBRATION_FS_HZ));
        unsigned mask=dma.ready_mask|dma.busy_mask;unsigned used=((mask&1u)?1u:0u)+((mask&2u)?1u:0u);if(used>peak_slots)peak_slots=used;
        if(block==0&&dma_buffer_take(&dma,&held)!=0){fprintf(stderr,"ERROR: completed DMA block unavailable\n");free(samples);return 7;}
    }
    clock_t dsp_start=clock();DspFeatures features={0};
    if(dma_buffer_take(&dma,&ready)!=0){fprintf(stderr,"ERROR: second DMA block unavailable\n");free(samples);return 8;}
    memcpy(samples,held->samples,n*sizeof(*samples));
    for(unsigned repeat=0;repeat<128;repeat++)dsp_process(samples,n,(float)VIBRATION_FS_HZ,&features);
    dma_buffer_release(&dma,held);
    memcpy(samples,ready->samples,n*sizeof(*samples));dsp_process(samples,n,(float)VIBRATION_FS_HZ,&features);dma_buffer_release(&dma,ready);
    double dsp_ms=(1000.0*(double)(clock()-dsp_start)/CLOCKS_PER_SEC)/129.0;
    float temp_c=0,current_a=0,mic_sample=0;double current_sq=0,mic_sq=0;
    for(unsigned i=0;i<2u*TEMPERATURE_FS_HZ;i++)if(temperature_sensor_read(&temperature,&temp_c)!=SENSOR_OK){fprintf(stderr,"ERROR: temperature sample invalid\n");free(samples);return 6;}
    for(unsigned i=0;i<CURRENT_FS_HZ;i++){float value;if(current_sensor_read(&current,&value)!=SENSOR_OK){fprintf(stderr,"ERROR: current sample invalid\n");free(samples);return 6;}current_sq+=(double)value*value;}
    current_a=(float)sqrt(current_sq/CURRENT_FS_HZ);
    for(unsigned i=0;i<MICROPHONE_FS_HZ/8u;i++){float value;if(microphone_read(&microphone,&value)!=SENSOR_OK){fprintf(stderr,"ERROR: microphone sample invalid\n");free(samples);return 6;}mic_sq+=(double)value*value;}
    mic_sample=(float)sqrt(mic_sq/(MICROPHONE_FS_HZ/8u));
    MonitorResult result=monitor_classify(&features,current_a,temp_c,mic_sample,shaft_hz);
    double workload_ms=1000.0*(double)(clock()-pipeline_start)/CLOCKS_PER_SEC;
    uint8_t frame[FRAME_SIZE];size_t frame_len=protocol_encode(frame,sizeof(frame),1u,(uint32_t)(2u*n*1000u/VIBRATION_FS_HZ),features.rms,temp_c,current_a,(uint8_t)result.fault,(uint8_t)result.health);
    if(frame_len==0){fprintf(stderr,"ERROR: telemetry frame encoding failed\n");free(samples);return 9;}
    uint32_t decoded_sequence=0;float decoded_rms=0;if(protocol_decode(frame,frame_len,&decoded_sequence,&decoded_rms)!=0||decoded_sequence!=1u||decoded_rms!=features.rms){fprintf(stderr,"ERROR: telemetry round-trip check failed\n");free(samples);return 10;}
    printf("scenario=%s fault=%s health=%s rms=%.4f dominant_hz=%.2f severity=%.2f\nhost_fft_mean_cpu_ms=%.3f host_sim_workload_cpu_ms=%.3f fft_trials=129 processing_deadline_ms=%.3f peak_buffer_utilization_pct=%.1f missed_samples=%u buffer_overflows=%u uart_roundtrip=PASS\nreason=%s\n",scenario,fault_name(result.fault),result.health==HEALTH_NORMAL?"NORMAL":result.health==HEALTH_WARNING?"WARNING":"CRITICAL",features.rms,features.dominant_hz,result.severity,dsp_ms,workload_ms,1000.0*(double)n/VIBRATION_FS_HZ,100.0*(double)peak_slots/2.0,dma.dropped_samples,dma.overflows,result.reason);
    printf("uart_frame=");for(size_t i=0;i<frame_len;i++)printf("%02X",frame[i]);printf("\n");
    microphone_shutdown(&microphone);current_sensor_shutdown(&current);temperature_sensor_shutdown(&temperature);vibration_sensor_shutdown(&vibration);free(samples);return 0;
}
