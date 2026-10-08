#include "stm32wb_device.h"
#include "zcl/general/zcl.identify.h"
#include "zcl/general/zcl.temp.meas.h"
#include "zcl/general/zcl.wcm.h"
#include "zcl/general/zcl.illum.meas.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.level.h"
#include "zcl/general/zcl.fan.h"
#include "zcl_values.h"
#include <string.h>

#define ROOM_ENDPOINT 1u
#define ROOM_HA_PROFILE 0x0104u
#define ROOM_TEMP_MIN (-4000)
#define ROOM_TEMP_MAX 12500
#define ROOM_HUMIDITY_CLUSTER 0x0405u
#define ROOM_DEVICE_SENSOR 0x0302u
#define ROOM_DEVICE_DIMMABLE_LIGHT 0x0101u
#define ROOM_DEVICE_HVAC 0x0300u

extern void board_light_set(bool on, uint8_t level);
extern void board_fan_set(bool on, uint8_t percent);

static bool add_endpoint(struct ZigBeeT *zb, enum room_zb_role role) {
    ZbApsmeAddEndpointReqT req;
    ZbApsmeAddEndpointConfT conf;
    memset(&req, 0, sizeof(req)); memset(&conf, 0, sizeof(conf));
    req.endpoint = ROOM_ENDPOINT;
    req.profileId = ROOM_HA_PROFILE;
    req.deviceId = role == ROOM_ZB_SENSOR ? ROOM_DEVICE_SENSOR :
                   role == ROOM_ZB_LIGHT ? ROOM_DEVICE_DIMMABLE_LIGHT : ROOM_DEVICE_HVAC;
    ZbZclAddEndpoint(zb, &req, &conf);
    return conf.status == ZB_STATUS_SUCCESS;
}

static enum ZclStatusCodeT light_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    (void)cluster; (void)src; struct room_zb_device *d = arg;
    d->actuator.on = true; if (d->actuator.level == 0u) d->actuator.level = 254u;
    board_light_set(true, d->actuator.level);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 1);
    return ZCL_STATUS_SUCCESS;
}
static enum ZclStatusCodeT light_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    (void)cluster; (void)src; struct room_zb_device *d = arg;
    d->actuator.on = false; board_light_set(false, 0u);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 0);
    return ZCL_STATUS_SUCCESS;
}
static enum ZclStatusCodeT light_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    struct room_zb_device *d = arg; return d->actuator.on ? light_off(cluster, src, arg) : light_on(cluster, src, arg);
}
static enum ZclStatusCodeT move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *src, void *arg) {
    (void)cluster; (void)src; struct room_zb_device *d = arg;
    d->actuator.level = req->level; d->actuator.on = req->level != 0u;
    board_light_set(d->actuator.on, d->actuator.level);
    (void)ZbZclAttrIntegerWrite(d->level, ZCL_LEVEL_ATTR_CURRLEVEL, req->level);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, d->actuator.on ? 1 : 0);
    return ZCL_STATUS_SUCCESS;
}
static enum ZclStatusCodeT fan_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    (void)cluster; (void)src; struct room_zb_device *d = arg;
    d->actuator.on = true; if (d->actuator.fan_percent == 0u) d->actuator.fan_percent = 100u;
    board_fan_set(true, d->actuator.fan_percent);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 1);
    (void)ZbZclAttrIntegerWrite(d->fan, ZCL_FAN_ATTR_MODE, ZCL_FAN_MODE_HI);
    return ZCL_STATUS_SUCCESS;
}
static enum ZclStatusCodeT fan_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    (void)cluster; (void)src; struct room_zb_device *d = arg;
    d->actuator.on = false; d->actuator.fan_percent = 0u; board_fan_set(false, 0u);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 0);
    (void)ZbZclAttrIntegerWrite(d->fan, ZCL_FAN_ATTR_MODE, ZCL_FAN_MODE_OFF);
    return ZCL_STATUS_SUCCESS;
}
static enum ZclStatusCodeT fan_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *src, void *arg) {
    struct room_zb_device *d = arg; return d->actuator.on ? fan_off(cluster, src, arg) : fan_on(cluster, src, arg);
}

bool room_zb_device_init(struct room_zb_device *d, struct ZigBeeT *zb, enum room_zb_role role) {
    if (d == NULL || zb == NULL) return false;
    memset(d, 0, sizeof(*d)); d->zb = zb; d->role = role;
    if (!add_endpoint(zb, role)) return false;
    d->identify = ZbZclIdentifyServerAlloc(zb, ROOM_ENDPOINT, NULL, NULL);
    if (d->identify == NULL) return false;
    ZbZclClusterEndpointRegister(d->identify);
    if (role == ROOM_ZB_SENSOR) {
        d->temperature = ZbZclTempMeasServerAlloc(zb, ROOM_ENDPOINT, ROOM_TEMP_MIN, ROOM_TEMP_MAX, 50u);
        d->humidity = ZbZclWaterContentMeasServerAlloc(zb, ROOM_ENDPOINT, (enum ZbZclClusterIdT)ROOM_HUMIDITY_CLUSTER, 0u, 10000u);
        d->illuminance = ZbZclIllumMeasServerAlloc(zb, ROOM_ENDPOINT, 0u, 65534u);
        if (d->temperature == NULL || d->humidity == NULL || d->illuminance == NULL) return false;
        ZbZclClusterEndpointRegister(d->temperature); ZbZclClusterEndpointRegister(d->humidity); ZbZclClusterEndpointRegister(d->illuminance);
        double temp_change = 50.0, humidity_change = 100.0, light_change = 100.0;
        (void)ZbZclAttrReportConfigDefault(d->temperature, ZCL_TEMP_MEAS_ATTR_MEAS_VAL, 5u, 30u, &temp_change);
        (void)ZbZclAttrReportConfigDefault(d->humidity, ZCL_WC_MEAS_ATTR_MEAS_VAL, 5u, 30u, &humidity_change);
        (void)ZbZclAttrReportConfigDefault(d->illuminance, ZCL_ILLUM_MEAS_ATTR_MEAS_VAL, 5u, 30u, &light_change);
        return true;
    }
    if (role == ROOM_ZB_LIGHT) {
        struct ZbZclOnOffServerCallbacksT onoff_cb;
        struct ZbZclLevelServerCallbacksT level_cb;
        memset(&onoff_cb, 0, sizeof(onoff_cb)); memset(&level_cb, 0, sizeof(level_cb));
        onoff_cb.on = light_on; onoff_cb.off = light_off; onoff_cb.toggle = light_toggle;
        level_cb.move_to_level = move_to_level;
        d->onoff = ZbZclOnOffServerAlloc(zb, ROOM_ENDPOINT, &onoff_cb, d);
        if (d->onoff == NULL) return false;
        d->level = ZbZclLevelServerAlloc(zb, ROOM_ENDPOINT, d->onoff, &level_cb, d);
        if (d->level == NULL) return false;
        ZbZclClusterEndpointRegister(d->onoff); ZbZclClusterEndpointRegister(d->level);
        (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 0);
        (void)ZbZclAttrIntegerWrite(d->level, ZCL_LEVEL_ATTR_CURRLEVEL, 254);
        d->actuator.level = 254u;
        return true;
    }
    struct ZbZclOnOffServerCallbacksT fan_cb;
    memset(&fan_cb, 0, sizeof(fan_cb)); fan_cb.on = fan_on; fan_cb.off = fan_off; fan_cb.toggle = fan_toggle;
    d->onoff = ZbZclOnOffServerAlloc(zb, ROOM_ENDPOINT, &fan_cb, d);
    d->fan = ZbZclFanServerAlloc(zb, ROOM_ENDPOINT);
    if (d->onoff == NULL || d->fan == NULL) return false;
    ZbZclClusterEndpointRegister(d->onoff); ZbZclClusterEndpointRegister(d->fan);
    (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, 0);
    (void)ZbZclAttrIntegerWrite(d->fan, ZCL_FAN_ATTR_MODE, ZCL_FAN_MODE_OFF);
    return true;
}

bool room_zb_publish_measurement(struct room_zb_device *d, const room_measurement_t *m) {
    if (d == NULL || m == NULL || d->role != ROOM_ZB_SENSOR) return false;
    enum ZclStatusCodeT a = ZbZclAttrIntegerWrite(d->temperature, ZCL_TEMP_MEAS_ATTR_MEAS_VAL, m->temperature_centi_c);
    enum ZclStatusCodeT b = ZbZclAttrIntegerWrite(d->humidity, ZCL_WC_MEAS_ATTR_MEAS_VAL, m->humidity_centi_pct);
    enum ZclStatusCodeT c = ZbZclAttrIntegerWrite(d->illuminance, ZCL_ILLUM_MEAS_ATTR_MEAS_VAL, room_lux_to_zcl_illuminance(m->illuminance_lux));
    return a == ZCL_STATUS_SUCCESS && b == ZCL_STATUS_SUCCESS && c == ZCL_STATUS_SUCCESS;
}

void room_zb_actuator_poll(struct room_zb_device *d) {
    if (d == NULL || d->role != ROOM_ZB_FAN || d->fan == NULL) return;
    enum ZclStatusCodeT status;
    long long mode = ZbZclAttrIntegerRead(d->fan, ZCL_FAN_ATTR_MODE, NULL, &status);
    if (status != ZCL_STATUS_SUCCESS) return;
    uint8_t pct = mode == ZCL_FAN_MODE_LOW ? 35u : mode == ZCL_FAN_MODE_MED ? 65u :
                  (mode == ZCL_FAN_MODE_HI || mode == ZCL_FAN_MODE_ON || mode == ZCL_FAN_MODE_AUTO || mode == ZCL_FAN_MODE_SMART) ? 100u : 0u;
    bool changed = d->actuator.fan_percent != pct || d->actuator.on != (pct != 0u);
    d->actuator.on = pct != 0u; d->actuator.fan_percent = pct;
    if (changed) {
        board_fan_set(d->actuator.on, pct);
        if (d->onoff != NULL) (void)ZbZclAttrIntegerWrite(d->onoff, ZCL_ONOFF_ATTR_ONOFF, d->actuator.on ? 1 : 0);
    }
}
