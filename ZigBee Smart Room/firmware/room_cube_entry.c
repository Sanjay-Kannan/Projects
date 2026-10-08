#include "room_cube_entry.h"
#include "device_config.h"
#include "room_task.h"
#include "stm32wb_network.h"
#include "app_debug.h"
extern I2C_HandleTypeDef hi2c1;
extern void room_cube_scheduler_start(struct ZigBeeT *zb);
void room_cube_application_start(struct ZigBeeT *zb) {
#if ROOM_DEVICE_ROLE == ROOM_ROLE_SENSOR
    const enum room_zb_role role = ROOM_ZB_SENSOR;
#elif ROOM_DEVICE_ROLE == ROOM_ROLE_LIGHT
    const enum room_zb_role role = ROOM_ZB_LIGHT;
#else
    const enum room_zb_role role = ROOM_ZB_FAN;
#endif
    if (!room_task_start(zb, role, &hi2c1)) { APP_DBG("[APP] Endpoint/device initialization failed"); return; }
    APP_DBG("[APP] Device clusters ready");
    room_cube_scheduler_start(zb);
}
