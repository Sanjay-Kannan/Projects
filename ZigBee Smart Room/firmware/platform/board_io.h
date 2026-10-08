#ifndef BOARD_IO_H
#define BOARD_IO_H
#include <stdbool.h>
#include <stdint.h>
#include "stm32wbxx_hal.h"
void board_io_init(void);
void board_light_set(bool on, uint8_t level);
void board_fan_set(bool on, uint8_t percent);
#endif
