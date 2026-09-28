"""Validation helpers for newline-delimited firmware JSON events."""
import json

EVENT_FIELDS = {
    "release": {"task", "job", "release_us", "deadline_us"},
    "start": {"task", "job", "time_us", "deadline_us"},
    "finish": {"task", "job", "time_us", "deadline_us", "elapsed_us", "deadline_miss"},
    "drop": {"task", "job", "time_us", "reason"},
    "telemetry": {"time_us", "temperature_c", "sensor_ok", "fan_duty"},
}


def parse_event(line: str) -> dict | None:
    """Parse a firmware line. Return None for comments/blank lines."""
    line = line.strip()
    if not line or line.startswith("#"):
        return None
    event = json.loads(line)
    kind = event.get("event")
    if kind not in EVENT_FIELDS:
        raise ValueError(f"unknown event type: {kind!r}")
    missing = EVENT_FIELDS[kind] - event.keys()
    if missing:
        raise ValueError(f"{kind} event missing fields: {', '.join(sorted(missing))}")
    return event
