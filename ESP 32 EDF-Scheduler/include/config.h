#pragma once

#include <stdint.h>
#include <freertos/FreeRTOS.h>

enum TaskId : uint8_t {
  TASK_SENSOR = 0,
  TASK_CONTROL,
  TASK_TELEMETRY,
};

struct TaskConfig {
  const char *name;
  uint32_t periodMs;
  uint32_t relativeDeadlineMs;
};

static constexpr TaskConfig TASKS[] = {
    {"sensor", 200, 150},
    {"control", 100, 100},
    {"telemetry", 1000, 900},
};
static constexpr uint8_t TASK_COUNT = sizeof(TASKS) / sizeof(TASKS[0]);
static constexpr uint32_t RUN_DURATION_SECONDS = 0;
// Each active job uses one persistent worker task; excess jobs are logged as drops.
static constexpr uint8_t MAX_ACTIVE_JOBS = 8;
static constexpr uint32_t JOB_STACK_WORDS = 3072;
static constexpr UBaseType_t EDF_LOW_PRIORITY = 2;
static constexpr UBaseType_t RELEASE_TASK_PRIORITY = 12;
static constexpr BaseType_t SCHEDULER_CORE = 1;
static_assert(EDF_LOW_PRIORITY + MAX_ACTIVE_JOBS <= RELEASE_TASK_PRIORITY,
              "Release tasks must outrank active EDF job tasks.");
static_assert(RELEASE_TASK_PRIORITY < configMAX_PRIORITIES,
              "Increase FreeRTOS configMAX_PRIORITIES for the configured task priorities.");

static constexpr uint8_t THERMISTOR_PIN = 34;
static constexpr uint8_t FAN_PWM_PIN = 25;
static constexpr uint8_t FAN_PWM_CHANNEL = 0;
static constexpr uint32_t FAN_PWM_FREQUENCY_HZ = 25000;
static constexpr uint8_t FAN_PWM_RESOLUTION_BITS = 8;

static constexpr float THERMISTOR_NOMINAL_OHMS = 10000.0f;
static constexpr float SERIES_RESISTOR_OHMS = 10000.0f;
static constexpr float THERMISTOR_BETA = 3950.0f;
static constexpr float THERMISTOR_NOMINAL_C = 25.0f;
static constexpr float FAN_OFF_BELOW_C = 29.0f;
static constexpr float FAN_FULL_AT_C = 43.0f;
static constexpr uint8_t FAN_MIN_START_DUTY = 80;
