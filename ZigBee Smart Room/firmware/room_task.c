#include "room_task.h"
#include "zigbee/stm32wb_device.h"
#include "env_app.h"
#include "board_io.h"
#include "stm32wbxx_hal.h"
#include <string.h>
static struct room_zb_device device;
static env_app_t environmental;
static bool ready;
static void report_values(const room_measurement_t *m, void *context) {
    struct room_zb_device *d = context;
    (void)room_zb_publish_measurement(d, m);
}
bool room_task_start(struct ZigBeeT *zb, enum room_zb_role role, I2C_HandleTypeDef *i2c) {
    board_io_init();
    struct ZbZclBasicServerDefaults basic;
    memset(&basic, 0, sizeof(basic));
    basic.app_version = 1u; basic.stack_version = 2u; basic.hw_version = 1u; basic.power_source = 0x01u;
    basic.mfr_name[0] = 11u; memcpy(&basic.mfr_name[1], "RoomLab EDU", 11u);
    const char *model = role == ROOM_ZB_SENSOR ? "Room Sensor" : role == ROOM_ZB_LIGHT ? "Room Light" : "Room Fan";
    size_t model_len = strlen(model); basic.model_name[0] = (uint8_t)model_len; memcpy(&basic.model_name[1], model, model_len);
    basic.sw_build_id[0] = 5u; memcpy(&basic.sw_build_id[1], "1.0.0", 5u);
    ZbZclBasicServerConfigDefaults(zb, &basic);
    ready = room_zb_device_init(&device, zb, role);
    if (!ready) return false;
    if (role == ROOM_ZB_SENSOR) env_app_init(&environmental, i2c, report_values, &device);
    return true;
}
void room_task_process(void) {
    if (!ready) return;
    if (device.role == ROOM_ZB_SENSOR) env_app_poll(&environmental, HAL_GetTick());
    if (device.role == ROOM_ZB_FAN) room_zb_actuator_poll(&device);
}
