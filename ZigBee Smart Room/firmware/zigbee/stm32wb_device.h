#ifndef STM32WB_DEVICE_H
#define STM32WB_DEVICE_H
#include "zigbee.h"
#include "zcl/zcl.h"
#include "measurement.h"
#include "actuator.h"
#include <stdbool.h>
enum room_zb_role { ROOM_ZB_SENSOR, ROOM_ZB_LIGHT, ROOM_ZB_FAN };
struct room_zb_device {
    struct ZigBeeT *zb;
    enum room_zb_role role;
    struct ZbZclClusterT *identify;
    struct ZbZclClusterT *temperature, *humidity, *illuminance;
    struct ZbZclClusterT *onoff, *level, *fan;
    room_actuator_t actuator;
};
bool room_zb_device_init(struct room_zb_device *device, struct ZigBeeT *zb, enum room_zb_role role);
bool room_zb_publish_measurement(struct room_zb_device *device, const room_measurement_t *value);
void room_zb_actuator_poll(struct room_zb_device *device);
#endif
