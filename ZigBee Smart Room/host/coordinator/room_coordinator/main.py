"""MQTT event adapter for Zigbee2MQTT normalized device state."""
import argparse, json, logging, math
import os
from pathlib import Path
from .control import Policy, RoomController

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", default=os.environ.get("ROOM_CONFIG", "host/coordinator/config.json"))
    args = parser.parse_args()
    cfg = json.loads(Path(args.config).read_text(encoding="utf-8"))
    logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
    try:
        import paho.mqtt.client as mqtt
    except ImportError as exc:
        raise SystemExit("Install runtime dependency with: pip install -e .") from exc
    controller = RoomController(Policy(**cfg["policy"]))
    base = cfg.get("base_topic", "zigbee2mqtt")
    names = cfg["devices"]
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="zigbee-smart-room")
    if cfg.get("username"):
        client.username_pw_set(cfg["username"], cfg.get("password"))
    def publish(device, payload):
        client.publish(f"{base}/{device}/set", json.dumps(payload), qos=1)
    def on_connect(client, userdata, flags, reason_code, properties):
        if reason_code == 0:
            logging.info("[MQTT] Connected; subscribing to sensor state")
            client.subscribe(f"{base}/{names['sensor']}", qos=1)
        else:
            logging.error("[MQTT] Connection rejected: %s", reason_code)
    def on_message(client, userdata, message):
        try:
            state = json.loads(message.payload.decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError):
            logging.warning("[MQTT] Ignoring malformed sensor payload")
            return
        if not isinstance(state, dict):
            logging.warning("[MQTT] Ignoring non-object sensor payload")
            return
        try:
            if "temperature" in state:
                temperature = float(state["temperature"])
                if math.isfinite(temperature):
                    result = controller.update_temperature(temperature)
                    if result is not None:
                        publish(names["fan"], {"fan_mode": result})
                        logging.info("[AUTO] fan_mode=%s", result)
            illuminance = state.get("illuminance_lux", state.get("illuminance"))
            if illuminance is not None:
                lux = float(illuminance)
                if math.isfinite(lux) and lux >= 0:
                    result = controller.update_illuminance(lux)
                    if result is not None:
                        publish(names["light"], {"state": "ON" if result else "OFF"})
                        logging.info("[AUTO] light=%s", "ON" if result else "OFF")
        except (TypeError, ValueError, OverflowError):
            logging.warning("[MQTT] Ignoring invalid numeric sensor value")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(os.environ.get("MQTT_HOST", cfg.get("host", "localhost")), int(os.environ.get("MQTT_PORT", cfg.get("port", 1883))), keepalive=60)
    client.loop_forever()

if __name__ == "__main__":
    main()
