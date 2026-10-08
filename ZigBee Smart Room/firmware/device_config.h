#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H
/* Build one firmware image per board by setting this to one of the values below. */
#define ROOM_ROLE_SENSOR 1
#define ROOM_ROLE_LIGHT  2
#define ROOM_ROLE_FAN    3
#ifndef ROOM_DEVICE_ROLE
#define ROOM_DEVICE_ROLE ROOM_ROLE_SENSOR
#endif
#if ROOM_DEVICE_ROLE < ROOM_ROLE_SENSOR || ROOM_DEVICE_ROLE > ROOM_ROLE_FAN
#error ROOM_DEVICE_ROLE must be ROOM_ROLE_SENSOR, ROOM_ROLE_LIGHT, or ROOM_ROLE_FAN
#endif
#endif
