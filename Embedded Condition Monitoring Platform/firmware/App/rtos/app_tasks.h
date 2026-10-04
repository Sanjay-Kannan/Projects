#ifndef APP_TASKS_H
#define APP_TASKS_H
/* Compile this target adapter only in a FreeRTOS firmware build. */
#ifdef USE_FREERTOS
void app_tasks_start(void);
#endif
#endif
