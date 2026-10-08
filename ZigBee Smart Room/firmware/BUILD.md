# Device firmware build and integration

The project targets NUCLEO-WB55RG with STM32CubeWB 1.20.0 Zigbee 3.0 and STM32CubeIDE. ST's Skeleton project supplies the M4/M0+ transport and middleware shell; its signed Zigbee FFD firmware is installed separately on CPU2. The board configuration is `platform/room_board.ioc`. Generate I2C1 on PB8/PB9 and TIM3 PWM on PB0/PB1 from that configuration.

Import the CubeWB `Zigbee_Skeleton` project for P-NUCLEO-WB55.Nucleo into CubeIDE. Run `scripts/install_cubewb_sources.ps1 -CubeWbProject <project-folder> -Role Sensor` (or `Light`/`Fan`) from PowerShell. This stages application sources, supplies the board `.ioc`, and writes the exact compiler/include settings to `ROOM_FIRMWARE_INTEGRATION.txt`. The Skeleton's Zigbee initialization starts the stack and prepares `zigbee_app_info.zb`; call `room_cube_application_start(zigbee_app_info.zb)` from its application-start sequencer task. The recurring RTC timer scheduler in `room_cube_scheduler.c` services sampling and fan mode updates outside interrupt context.

Build three images by setting `ROOM_DEVICE_ROLE` to `1`, `2`, or `3` in the compiler symbols for sensor, light, or fan. Keep the coordinator external: the Sonoff ZBDongle-P is managed by Zigbee2MQTT and must be the only coordinator for the PAN. `stm32wb_network.c` joins channel 15 as a mains-powered, non-sleepy end device. Network formation, permit join, host device discovery, and MQTT translation are handled by Zigbee2MQTT.

The core stack initialization uses ST's CubeWB Skeleton for release-matched IPCC, SHCI, FUS, and low-power initialization. Install the signed wireless coprocessor firmware for the selected CubeWB release on CPU2.
