#include <Arduino.h>
#include <esp_arduino_version.h>
#include <esp_timer.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "config.h"

struct Job {
  uint8_t taskIndex;
  uint32_t jobNumber;
  int64_t releaseUs;
  int64_t deadlineUs;
};

struct JobSlot {
  bool active;
  Job job;
  TaskHandle_t handle;
};

static JobSlot jobSlots[MAX_ACTIVE_JOBS];
static SemaphoreHandle_t schedulerMutex;
static SemaphoreHandle_t logMutex;
static portMUX_TYPE sensorMux = portMUX_INITIALIZER_UNLOCKED;
static float measuredTemperatureC = 0.0f;
static bool sensorValid = false;
static uint8_t fanDuty = 0;
static uint8_t fanTargetDuty = 0;
static int64_t fanStartUntilUs = 0;
static volatile bool stopping = false;

static void rescheduleJobs();

static int64_t nowUs() {
  return esp_timer_get_time();
}

static bool setFanDuty(uint8_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  return ledcWrite(FAN_PWM_PIN, duty);
#else
  ledcWrite(FAN_PWM_CHANNEL, duty);
  return true;
#endif
}

static bool configureFanPwm() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  return ledcAttach(FAN_PWM_PIN, FAN_PWM_FREQUENCY_HZ, FAN_PWM_RESOLUTION_BITS);
#else
  const double frequency = ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQUENCY_HZ,
                                     FAN_PWM_RESOLUTION_BITS);
  ledcAttachPin(FAN_PWM_PIN, FAN_PWM_CHANNEL);
  return frequency > 0;
#endif
}

static bool readTemperature(float &temperatureC) {
  uint32_t totalMillivolts = 0;
  constexpr uint8_t sampleCount = 8;
  for (uint8_t i = 0; i < sampleCount; ++i) {
    totalMillivolts += analogReadMilliVolts(THERMISTOR_PIN);
    delayMicroseconds(250);
  }

  const float voltageMv = static_cast<float>(totalMillivolts) / sampleCount;
  if (voltageMv <= 5.0f || voltageMv >= 3295.0f) {
    return false;
  }

  // Divider wiring: 3.3 V -> fixed resistor -> ADC node -> NTC -> GND.
  const float thermistorOhms = SERIES_RESISTOR_OHMS * voltageMv / (3300.0f - voltageMv);
  const float nominalKelvin = THERMISTOR_NOMINAL_C + 273.15f;
  const float inverseKelvin = (1.0f / nominalKelvin) +
      (logf(thermistorOhms / THERMISTOR_NOMINAL_OHMS) / THERMISTOR_BETA);
  temperatureC = (1.0f / inverseKelvin) - 273.15f;
  return isfinite(temperatureC) && temperatureC >= -40.0f && temperatureC <= 125.0f;
}

static uint8_t temperatureToFanDuty(float temperatureC, uint8_t currentDuty) {
  if (temperatureC < FAN_OFF_BELOW_C) {
    return 0;
  }
  if (temperatureC >= FAN_FULL_AT_C) {
    return 255;
  }
  if (temperatureC < FAN_OFF_BELOW_C + 1.0f) {
    return currentDuty;
  }

  const float fraction = (temperatureC - FAN_OFF_BELOW_C) /
                         (FAN_FULL_AT_C - FAN_OFF_BELOW_C);
  const float range = 255.0f - FAN_MIN_START_DUTY;
  return static_cast<uint8_t>(FAN_MIN_START_DUTY + fraction * range);
}

static void emitRelease(const Job &job) {
  const TaskConfig &cfg = TASKS[job.taskIndex];
  xSemaphoreTake(logMutex, portMAX_DELAY);
  Serial.printf("{\"event\":\"release\",\"task\":\"%s\",\"job\":%lu,\"release_us\":%lld,\"deadline_us\":%lld}\n",
                cfg.name, static_cast<unsigned long>(job.jobNumber),
                static_cast<long long>(job.releaseUs), static_cast<long long>(job.deadlineUs));
  xSemaphoreGive(logMutex);
}

static void emitStart(const Job &job, int64_t startUs) {
  xSemaphoreTake(logMutex, portMAX_DELAY);
  Serial.printf("{\"event\":\"start\",\"task\":\"%s\",\"job\":%lu,\"time_us\":%lld,\"deadline_us\":%lld}\n",
                TASKS[job.taskIndex].name, static_cast<unsigned long>(job.jobNumber),
                static_cast<long long>(startUs), static_cast<long long>(job.deadlineUs));
  xSemaphoreGive(logMutex);
}

static void emitFinish(const Job &job, int64_t startUs, int64_t finishUs) {
  const bool missed = finishUs > job.deadlineUs;
  xSemaphoreTake(logMutex, portMAX_DELAY);
  Serial.printf("{\"event\":\"finish\",\"task\":\"%s\",\"job\":%lu,\"time_us\":%lld,\"deadline_us\":%lld,\"elapsed_us\":%lld,\"deadline_miss\":%s}\n",
                TASKS[job.taskIndex].name, static_cast<unsigned long>(job.jobNumber),
                static_cast<long long>(finishUs), static_cast<long long>(job.deadlineUs),
                static_cast<long long>(finishUs - startUs), missed ? "true" : "false");
  xSemaphoreGive(logMutex);
}

static void emitDrop(const Job &job) {
  xSemaphoreTake(logMutex, portMAX_DELAY);
  Serial.printf("{\"event\":\"drop\",\"task\":\"%s\",\"job\":%lu,\"time_us\":%lld,\"reason\":\"job_pool_full\"}\n",
                TASKS[job.taskIndex].name, static_cast<unsigned long>(job.jobNumber),
                static_cast<long long>(nowUs()));
  xSemaphoreGive(logMutex);
}

static void emitTelemetry() {
  float temperature;
  bool valid;
  uint8_t duty;
  portENTER_CRITICAL(&sensorMux);
  temperature = measuredTemperatureC;
  valid = sensorValid;
  duty = fanDuty;
  portEXIT_CRITICAL(&sensorMux);

  xSemaphoreTake(logMutex, portMAX_DELAY);
  if (valid) {
    Serial.printf("{\"event\":\"telemetry\",\"time_us\":%lld,\"temperature_c\":%.2f,\"sensor_ok\":true,\"fan_duty\":%u}\n",
                  static_cast<long long>(nowUs()), temperature, duty);
  } else {
    Serial.printf("{\"event\":\"telemetry\",\"time_us\":%lld,\"temperature_c\":null,\"sensor_ok\":false,\"fan_duty\":%u}\n",
                  static_cast<long long>(nowUs()), duty);
  }
  xSemaphoreGive(logMutex);
}

static void runTask(const Job &job) {
  switch (job.taskIndex) {
    case TASK_SENSOR: {
      float temperature;
      const bool valid = readTemperature(temperature);
      portENTER_CRITICAL(&sensorMux);
      sensorValid = valid;
      if (valid) {
        measuredTemperatureC = temperature;
      }
      portEXIT_CRITICAL(&sensorMux);
      break;
    }
    case TASK_CONTROL: {
      float temperature;
      bool valid;
      uint8_t currentDuty;
      uint8_t currentTargetDuty;
      int64_t startUntil;
      portENTER_CRITICAL(&sensorMux);
      temperature = measuredTemperatureC;
      valid = sensorValid;
      currentDuty = fanDuty;
      currentTargetDuty = fanTargetDuty;
      startUntil = fanStartUntilUs;
      portEXIT_CRITICAL(&sensorMux);

      const uint8_t targetDuty = valid ? temperatureToFanDuty(temperature, currentTargetDuty) : 255;
      uint8_t outputDuty = targetDuty;
      const int64_t now = nowUs();
      if (targetDuty == 0) {
        startUntil = 0;
      } else if (currentDuty == 0) {
        startUntil = now + 300000;
      }
      if (startUntil > now) {
        outputDuty = 255;
      }
      if (setFanDuty(outputDuty)) {
        portENTER_CRITICAL(&sensorMux);
        fanDuty = outputDuty;
        fanTargetDuty = targetDuty;
        fanStartUntilUs = startUntil;
        portEXIT_CRITICAL(&sensorMux);
      }
      break;
    }
    case TASK_TELEMETRY:
      emitTelemetry();
      break;
  }
}

static void releaseTask(void *arg) {
  const uint8_t taskIndex = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(arg));
  const TaskConfig &cfg = TASKS[taskIndex];
  TickType_t lastWake = xTaskGetTickCount();
  uint32_t jobNumber = 0;

  vTaskDelay(pdMS_TO_TICKS((taskIndex * 7U) % cfg.periodMs));
  lastWake = xTaskGetTickCount();
  while (!stopping) {
    const int64_t release = nowUs();
    Job job{taskIndex, ++jobNumber, release,
            release + static_cast<int64_t>(cfg.relativeDeadlineMs) * 1000};
    emitRelease(job);
    xSemaphoreTake(schedulerMutex, portMAX_DELAY);
    int freeSlot = -1;
    for (uint8_t i = 0; i < MAX_ACTIVE_JOBS; ++i) {
      if (!jobSlots[i].active) {
        freeSlot = i;
        break;
      }
    }
    if (freeSlot < 0) {
      xSemaphoreGive(schedulerMutex);
      emitDrop(job);
    } else {
      JobSlot &slot = jobSlots[freeSlot];
      slot.job = job;
      slot.active = true;
      rescheduleJobs();
      xSemaphoreGive(schedulerMutex);
      xTaskNotifyGive(slot.handle);
    }
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(cfg.periodMs));
  }
  vTaskDelete(nullptr);
}

static void rescheduleJobs() {
  uint8_t order[MAX_ACTIVE_JOBS];
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_ACTIVE_JOBS; ++i) {
    if (jobSlots[i].active) {
      order[count++] = i;
    }
  }

  for (uint8_t i = 1; i < count; ++i) {
    const uint8_t candidate = order[i];
    uint8_t j = i;
    while (j > 0) {
      const Job &previous = jobSlots[order[j - 1]].job;
      const Job &current = jobSlots[candidate].job;
      if (previous.deadlineUs < current.deadlineUs ||
          (previous.deadlineUs == current.deadlineUs && previous.releaseUs <= current.releaseUs)) {
        break;
      }
      order[j] = order[j - 1];
      --j;
    }
    order[j] = candidate;
  }

  vTaskSuspendAll();
  for (uint8_t rank = 0; rank < count; ++rank) {
    const UBaseType_t priority = EDF_LOW_PRIORITY + count - rank;
    vTaskPrioritySet(jobSlots[order[rank]].handle, priority);
  }
  xTaskResumeAll();
}

static void jobWorker(void *arg) {
  const uint8_t slotIndex = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(arg));
  JobSlot &slot = jobSlots[slotIndex];
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    const Job job = slot.job;
    emitStart(job, nowUs());
    const int64_t start = nowUs();
    runTask(job);
    const int64_t finish = nowUs();
    emitFinish(job, start, finish);

    xSemaphoreTake(schedulerMutex, portMAX_DELAY);
    slot.active = false;
    rescheduleJobs();
    xSemaphoreGive(schedulerMutex);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  analogReadResolution(12);
  analogSetPinAttenuation(THERMISTOR_PIN, ADC_11db);

  if (!configureFanPwm()) {
    Serial.println("# ERROR: failed to configure fan PWM");
    for (;;) delay(1000);
  }
  setFanDuty(0);

  schedulerMutex = xSemaphoreCreateMutex();
  logMutex = xSemaphoreCreateMutex();
  if (schedulerMutex == nullptr || logMutex == nullptr) {
    Serial.println("# ERROR: failed to allocate scheduler resources");
    for (;;) delay(1000);
  }

  Serial.println("# NTC temperature monitor and PWM fan controller (EDF)");
  Serial.printf("# thermistor_gpio=%u fan_pwm_gpio=%u pwm_hz=%lu\n",
                THERMISTOR_PIN, FAN_PWM_PIN, static_cast<unsigned long>(FAN_PWM_FREQUENCY_HZ));
  for (uint8_t i = 0; i < MAX_ACTIVE_JOBS; ++i) {
    jobSlots[i].active = false;
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "edf_job_%u", i);
    if (xTaskCreatePinnedToCore(jobWorker, taskName, JOB_STACK_WORDS,
                                reinterpret_cast<void *>(static_cast<uintptr_t>(i)),
                                EDF_LOW_PRIORITY, &jobSlots[i].handle, SCHEDULER_CORE) != pdPASS) {
      Serial.printf("# ERROR: failed to create job worker %u\n", i);
      for (;;) delay(1000);
    }
  }
  for (uint8_t i = 0; i < TASK_COUNT; ++i) {
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "release_%u", i);
    if (xTaskCreatePinnedToCore(releaseTask, taskName, 2048,
                                reinterpret_cast<void *>(static_cast<uintptr_t>(i)),
                                RELEASE_TASK_PRIORITY, nullptr, SCHEDULER_CORE) != pdPASS) {
      Serial.printf("# ERROR: failed to create release task %u\n", i);
      for (;;) delay(1000);
    }
  }
#if RUN_DURATION_SECONDS > 0
  vTaskDelay(pdMS_TO_TICKS(RUN_DURATION_SECONDS * 1000UL));
  stopping = true;
  Serial.println("# run duration elapsed; reset board to restart");
#endif
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
