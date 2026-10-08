#include "room_task.h"
#include "stm32wbxx_hal.h"
#include "stm32_timer.h"
#include "app_debug.h"
#include "stm32wb_network.h"
static UTIL_TIMER_Object_t room_service_timer;
static struct ZigBeeT *room_zigbee;
static bool network_joined;
static void room_service_callback(void *context) {
    (void)context;
    if (!network_joined) {
        if (room_zb_join_network(room_zigbee) == ZB_STATUS_SUCCESS) {
            network_joined = true;
            APP_DBG("[APP] Device joined; starting sampling and actuator service");
        } else {
            APP_DBG("[ZB] Join retry scheduled");
        }
    } else {
        room_task_process();
    }
    (void)UTIL_TIMER_SetPeriod(&room_service_timer, network_joined ? 1000u : 5000u);
    (void)UTIL_TIMER_Start(&room_service_timer);
}
void room_cube_scheduler_start(struct ZigBeeT *zb) {
    room_zigbee = zb;
    network_joined = false;
    UTIL_TIMER_Create(&room_service_timer, 100u, UTIL_TIMER_ONESHOT, room_service_callback, NULL);
    if (UTIL_TIMER_Start(&room_service_timer) != UTIL_TIMER_OK) APP_DBG("[APP] Could not start room service timer");
}
