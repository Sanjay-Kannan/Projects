# Reproducible build and flash

## Component versions

- MCU board: ST NUCLEO-WB55RG (STM32WB55RGV6)
- MCU package: STM32CubeWB 1.20.0 / STM32CubeIDE-supported Zigbee Skeleton
- Wireless stack: ST STM32WB Zigbee FFD signed CPU2 firmware matching the CubeWB package
- Coordinator: Sonoff ZBDongle-P (TI CC2652P), Zigbee2MQTT 2.14.0, Mosquitto 2.x
- Host automation: Python 3.10+ and paho-mqtt 2.x

ST's signed CPU2 Zigbee stack and CubeWB middleware are distributed by ST; this project does not redistribute them. Keep the CPU2 binary, CubeWB source package, and CubeIDE project from the same CubeWB release.

## Host coordinator and policy service

On Linux, set `ZIGBEE_SERIAL_PORT` in `.env` to the exact stable USB serial path. Run `docker compose up -d`. The Zigbee2MQTT frontend is bound to `localhost:8080`, Mosquitto is bound to `localhost:1883`, the room policy service starts automatically, and PAN credentials persist under `host/zigbee2mqtt/data`. Pair the nodes and assign the friendly names in `host/coordinator/config.json`.

## STM32 node images

1. Install STM32CubeIDE, STM32CubeMX, STM32CubeProgrammer, ST-LINK drivers, and STM32CubeWB 1.20.0 including the Zigbee stack package.
2. Open `firmware/platform/room_board.ioc` in CubeMX and generate STM32CubeIDE peripheral initialization for the NUCLEO-WB55RG. I2C1 uses PB8/PB9; TIM3_CH3/CH4 use PB0/PB1.
3. Import the CubeWB `Zigbee_Skeleton` for the P-NUCLEO-WB55.Nucleo. Run `scripts/install_cubewb_sources.ps1 -CubeWbProject <project-folder> -Role Sensor` (or `Light`/`Fan`) and follow the generated `ROOM_FIRMWARE_INTEGRATION.txt`. Connect `room_cube_application_start(zigbee_app_info.zb)` from the skeleton's application-start task after `ZbInit()` has completed; do not also register duplicate application endpoints.
4. Build three application images with `ROOM_DEVICE_ROLE=1` (sensor), `2` (light), and `3` (fan). Flash the matching image to each Nucleo board.
5. Install the release-matched encrypted Zigbee FFD binary on CPU2 using CubeProgrammer/FUS, as directed by ST's CubeWB release notes.

The coordinator is the CC2652P adapter, not a fourth STM32 image. CubeIDE places ELF/HEX/BIN artifacts in its build directory.
