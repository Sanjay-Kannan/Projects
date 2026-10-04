#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <stdint.h>

typedef struct { void *payload; uint32_t sequence; uint64_t timestamp_us; } AppMessage;
extern int app_acquisition_step(AppMessage *message);
extern int app_processing_step(AppMessage *message);
extern int app_monitoring_step(AppMessage *message);
extern void app_communication_step(const AppMessage *message);
extern void app_logging_step(const AppMessage *message);

static QueueHandle_t acquisition_queue, feature_queue, health_queue, logging_queue;
static SemaphoreHandle_t log_mutex;
static EventGroupHandle_t app_events;
#define EVENT_ACQUISITION_ERROR (1u<<0)
#define EVENT_PROCESSING_ERROR (1u<<1)
#define TASK_STACK_WORDS 768u

static void acquisition_task(void *arg){(void)arg;for(;;){AppMessage m;if(app_acquisition_step(&m)!=0){xEventGroupSetBits(app_events,EVENT_ACQUISITION_ERROR);vTaskDelay(pdMS_TO_TICKS(1));continue;}if(xQueueSend(acquisition_queue,&m,0)!=pdPASS)xEventGroupSetBits(app_events,EVENT_ACQUISITION_ERROR);}}
static void processing_task(void *arg){(void)arg;for(;;){AppMessage m;if(xQueueReceive(acquisition_queue,&m,portMAX_DELAY)!=pdPASS)continue;if(app_processing_step(&m)!=0){xEventGroupSetBits(app_events,EVENT_PROCESSING_ERROR);continue;}if(xQueueSend(feature_queue,&m,0)!=pdPASS)xEventGroupSetBits(app_events,EVENT_PROCESSING_ERROR);}}
static void monitoring_task(void *arg){(void)arg;for(;;){AppMessage m;if(xQueueReceive(feature_queue,&m,portMAX_DELAY)!=pdPASS)continue;if(app_monitoring_step(&m)!=0){xEventGroupSetBits(app_events,EVENT_PROCESSING_ERROR);continue;}if(xQueueSend(health_queue,&m,0)!=pdPASS||xQueueSend(logging_queue,&m,0)!=pdPASS)xEventGroupSetBits(app_events,EVENT_PROCESSING_ERROR);}}
static void communication_task(void *arg){(void)arg;for(;;){AppMessage m;if(xQueueReceive(health_queue,&m,portMAX_DELAY)==pdPASS)app_communication_step(&m);}}
static void logging_task(void *arg){(void)arg;for(;;){AppMessage m;if(xQueueReceive(logging_queue,&m,portMAX_DELAY)==pdPASS&&xSemaphoreTake(log_mutex,portMAX_DELAY)==pdPASS){app_logging_step(&m);xSemaphoreGive(log_mutex);}}}

void app_tasks_start(void){
    acquisition_queue=xQueueCreate(2,sizeof(AppMessage));feature_queue=xQueueCreate(2,sizeof(AppMessage));health_queue=xQueueCreate(4,sizeof(AppMessage));logging_queue=xQueueCreate(4,sizeof(AppMessage));
    log_mutex=xSemaphoreCreateMutex();app_events=xEventGroupCreate();
    configASSERT(acquisition_queue&&feature_queue&&health_queue&&logging_queue&&log_mutex&&app_events);
    configASSERT(xTaskCreate(acquisition_task,"acquire",TASK_STACK_WORDS,0,configMAX_PRIORITIES-2,0)==pdPASS);
    configASSERT(xTaskCreate(processing_task,"dsp",TASK_STACK_WORDS,0,configMAX_PRIORITIES-3,0)==pdPASS);
    configASSERT(xTaskCreate(monitoring_task,"monitor",TASK_STACK_WORDS,0,configMAX_PRIORITIES-4,0)==pdPASS);
    configASSERT(xTaskCreate(communication_task,"uart",TASK_STACK_WORDS,0,tskIDLE_PRIORITY+1,0)==pdPASS);
    configASSERT(xTaskCreate(logging_task,"log",TASK_STACK_WORDS,0,tskIDLE_PRIORITY+1,0)==pdPASS);
}
#endif
