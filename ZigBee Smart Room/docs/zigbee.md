# Zigbee concepts in this project

- **Coordinator:** the CC2652P adapter running Zigbee2MQTT forms this PAN and acts as trust center. It assigns network addresses, manages joining, and bridges ZCL traffic to MQTT.
- **Router:** an always-awake Zigbee node that forwards packets and can accept children. This small topology has no router nodes.
- **End device:** the three STM32WB nodes. They do not route. The development boards are USB-powered and configured as non-sleepy end devices so light and fan commands remain responsive. A battery design would use sleepy-end-device poll control and different reporting intervals.
- **PAN:** the network identified by its PAN ID and extended PAN ID; Zigbee2MQTT generates and stores these.
- **Network address:** 16-bit short address assigned within a PAN; it may change after rejoin.
- **IEEE address:** a radio's 64-bit extended address, used as a stable device identity.
- **Endpoint:** logical application interface; this project uses endpoint 1 on each STM32 device.
- **Cluster:** standard group of related attributes and commands.
- **Attribute:** typed state such as measured temperature or OnOff.
- **Command:** action such as On, Off, Toggle, MoveToLevel. CubeWB cluster callbacks apply commands to hardware.
- **Reporting:** attribute updates from a server cluster to its coordinator/client. Sensor measurement cluster reporting defaults are configured with five-second minimum and thirty-second maximum intervals; the coordinator can configure reporting over the air.
- **Binding:** persistent source cluster to destination endpoint association. The bridge addresses devices and configures reporting; application code does not implement a custom binding scheme.
- **Commissioning:** opening permit join in Zigbee2MQTT, joining nodes, endpoint discovery, and standard reporting configuration.
- **Rejoin:** the stack attempts recovery while the device retains network state; Zigbee2MQTT maintains coordinator network state on disk. A cold boot after loss of MCU application-side network persistence may require joining to be reopened.
- **Zigbee Pro:** the network layer, addressing, routing, and security stack supplied by the coordinator and CubeWB.
- **ZHA:** the legacy Home Automation application profile ID 0x0104 used by these endpoints.
- **ZCL:** the application layer defining this project's clusters, attributes, reporting, and commands.

Application code handles sensors, PWM, and policy. It does not implement 802.15.4 frames, Zigbee routing, cryptography, or ZCL frame encoding.
