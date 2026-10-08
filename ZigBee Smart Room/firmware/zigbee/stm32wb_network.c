#include "stm32wb_network.h"
#include "app_debug.h"

enum ZbStatusCodeT room_zb_join_network(struct ZigBeeT *zb) {
    struct ZbStartupT config;
    if (zb == NULL) return ZB_STATUS_FAILURE;
    ZbStartupConfigGetProDefaults(&config);
    config.startupControl = ZbStartTypeJoin;
    /* Mains-powered non-sleepy end device: do not route, keep radio available for commands. */
    config.capability &= (uint8_t)~MCP_ASSOC_CAP_DEV_TYPE;
    config.channelList.count = 1u;
    config.channelList.list[0].page = 0u;
    config.channelList.list[0].channelMask = (uint32_t)(1UL << 15);
    config.panId = 0xffffu;
    config.extendedPanId = 0u; /* Join a permitted network on the selected channel. */
    APP_DBG("[ZB] Joining network on channel 15");
    enum ZbStatusCodeT status = ZbStartupWait(zb, &config);
    if (status == ZB_STATUS_SUCCESS) APP_DBG("[ZB] Joined network");
    else APP_DBG("[ZB] Startup failed: 0x%02x", status);
    return status;
}
