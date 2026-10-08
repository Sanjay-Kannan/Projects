# Coordinator

The network coordinator is the Sonoff ZBDongle-P (TI CC2652P) managed by Zigbee2MQTT on the host. `compose.yaml` launches Zigbee2MQTT and Mosquitto with the coordinator frontend bound to localhost. The adapter is the only coordinator on this PAN. The STM32WB firmware images are end devices.
