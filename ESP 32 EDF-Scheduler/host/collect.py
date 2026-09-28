"""Capture newline-delimited JSON events from an ESP32 serial port."""
import argparse
import json
import time

import serial

from .events import parse_event


def collect(port: str, output: str, baud: int = 115200, duration: float | None = None) -> None:
    started = time.monotonic()
    with serial.Serial(port, baudrate=baud, timeout=1) as connection, open(output, "a", encoding="utf-8") as log:
        print(f"Listening on {port} at {baud} baud; writing {output}")
        while duration is None or time.monotonic() - started < duration:
            raw = connection.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="replace").strip()
            try:
                event = parse_event(line)
            except (ValueError, json.JSONDecodeError) as exc:
                print(f"Skipping malformed line: {exc}")
                continue
            if event is None:
                print(line)
                continue
            log.write(json.dumps(event, separators=(",", ":")) + "\n")
            log.flush()
            if event["event"] in {"finish", "drop"}:
                print(f"{event['event']:6} {event['task']} job={event['job']}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="serial port, e.g. COM5 or /dev/ttyUSB0")
    parser.add_argument("--output", default="run.jsonl")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--duration", type=float, help="capture duration in seconds; omit to run until Ctrl+C")
    args = parser.parse_args()
    try:
        collect(args.port, args.output, args.baud, args.duration)
    except KeyboardInterrupt:
        print("\nCapture stopped")


if __name__ == "__main__":
    main()
