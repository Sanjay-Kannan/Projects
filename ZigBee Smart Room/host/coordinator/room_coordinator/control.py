"""Pure policy logic shared by the MQTT service and unit tests."""
from dataclasses import dataclass

@dataclass(frozen=True)
class Policy:
    fan_on_c: float = 28.0
    fan_off_c: float = 26.0
    light_on_lux: float = 100.0
    light_off_lux: float = 140.0

class RoomController:
    def __init__(self, policy: Policy = Policy()):
        if policy.fan_off_c >= policy.fan_on_c or policy.light_on_lux >= policy.light_off_lux:
            raise ValueError("hysteresis off threshold must be lower/higher as appropriate")
        self.policy = policy
        self.fan_on = False
        self.light_on = False

    def update_temperature(self, celsius: float) -> str | None:
        if not -40.0 <= celsius <= 125.0:
            return None
        if celsius >= self.policy.fan_on_c:
            self.fan_on = True
        elif celsius <= self.policy.fan_off_c:
            self.fan_on = False
        if not self.fan_on:
            return "off"
        return "high" if celsius >= 32.0 else "medium" if celsius >= 29.0 else "low"

    def update_illuminance(self, lux: float) -> bool | None:
        if not 0.0 <= lux <= 100000.0:
            return None
        if lux < self.policy.light_on_lux:
            self.light_on = True
        elif lux > self.policy.light_off_lux:
            self.light_on = False
        return self.light_on
