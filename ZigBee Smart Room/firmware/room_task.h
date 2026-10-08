#ifndef ROOM_TASK_H
#define ROOM_TASK_H
#include "stm32wb_device.h"
#include "stm32wbxx_hal.h"
bool room_task_start(struct ZigBeeT *zb, enum room_zb_role role, I2C_HandleTypeDef *i2c);
void room_task_process(void);
#endif
