# Commissioning and operation

1. Connect the Sonoff ZBDongle-P to a Linux host and set `ZIGBEE_SERIAL_PORT` in `.env` to its stable `/dev/serial/by-id/...` path.
2. Run `docker compose up -d`; Zigbee2MQTT forms a secured PAN on channel 15 and stores generated network credentials under `host/zigbee2mqtt/data`. Joining is disabled by default.
3. Open `http://localhost:8080`, enable permit join, then power each STM32 node. Assign friendly names `room_sensor`, `smart_light`, and `smart_fan`. Close permit join after onboarding.
4. Confirm endpoint 1 cluster discovery in the Zigbee2MQTT device page. Sensor values report using standard measurement clusters.
5. The Compose deployment starts the room automation service automatically. Use the Zigbee2MQTT frontend to control the light, brightness, and fan mode manually.
6. If a node loses the coordinator temporarily, the stack attempts to recover its network connection. Zigbee2MQTT retains PAN state across restarts; preserve its data volume.

Expected log format from the device application (illustrative, not a captured hardware log):

```text
[ZB] Joining network on channel 15
[ZB] Network started/joined
[APP] Device ready
```

The coordinator's actual join and endpoint discovery logs are produced by Zigbee2MQTT. The MQTT service assumes the friendly names above.
