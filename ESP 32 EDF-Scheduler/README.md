# ESP32 Temperature-Controlled Fan with EDF Scheduling

This firmware reads a 10 kΩ NTC thermistor and controls a low-voltage brushed DC fan with PWM. Three periodic FreeRTOS release tasks create sensor, fan-control, and telemetry jobs. Each active job runs in its own persistent FreeRTOS task. The scheduler assigns priorities from absolute deadlines, so a newly released earlier-deadline job preempts a later-deadline job. Completion times, deadline misses, and pool drops are logged to Serial as JSON lines.

## Hardware

This pinout targets a classic ESP32 DevKit (`esp32dev`):

| Part | Connection |
| --- | --- |
| 10 kΩ fixed resistor | 3V3 to GPIO34 |
| 10 kΩ NTC thermistor, B=3950 | GPIO34 to GND |
| Logic-level N-channel MOSFET | Gate from GPIO25 through 100 Ω; source to GND; drain to fan negative |
| Brushed DC fan | Positive to its rated external supply; negative to MOSFET drain |
| Gate pull-down | 10 kΩ from MOSFET gate to GND |
| Flyback diode | Across the brushed motor: cathode to fan supply positive, anode to fan negative |

Connect the external fan supply ground to ESP32 GND. Use a MOSFET that fully switches on with a 3.3 V gate signal. Do not power the fan from an ESP32 GPIO. The thermistor divider is wired as 3V3 → fixed resistor → GPIO34 → thermistor → GND. Keep the ADC node between 0 and 3.3 V.

## Build and upload

Install PlatformIO and open this folder as a project. The default target is `esp32dev`:

```sh
pio run
pio run -t upload
pio device monitor -b 115200
```

If your board is not a classic ESP32 DevKit, select its PlatformIO board ID in `platformio.ini` and check that the configured GPIOs exist on that board. The thermistor, PWM, task periods, deadlines, and temperature thresholds are in `include/config.h`.

The fan is off below 29 °C, gets a brief full-power start pulse when it turns on, then ramps from its minimum running duty toward full duty by 43 °C. If the thermistor reading is open, shorted, or outside the configured range, the controller requests full fan duty.

## Capture and analyze

Install Python 3.10 or later and the host dependencies:

```sh
python -m venv .venv
# Windows: .venv\Scripts\activate
# macOS/Linux: source .venv/bin/activate
pip install -r host/requirements.txt
```

Capture a one-minute run (replace `COM5` with the ESP32 serial port):

```sh
python -m host.collect --port COM5 --output run.jsonl --duration 60
python -m host.analyze run.jsonl --plot schedule.png
```

On macOS or Linux, use a device path such as `/dev/ttyUSB0`. Omit `--duration` to collect until Ctrl+C. The report includes elapsed and response times, completed and dropped job counts, and deadline misses. Telemetry events include measured temperature, sensor status, and fan PWM duty. Run the host tests with `pytest`.

## Scheduling model

All scheduler and job tasks are pinned to one ESP32 core. Active jobs receive distinct FreeRTOS priorities ordered by absolute deadline. When a new job has an earlier deadline, its priority is raised above the running job and FreeRTOS preempts the running job. Priorities are recalculated whenever a job is released or completed. Up to eight jobs can be active; additional releases are reported as drops. A job's logged start-to-finish elapsed time can include time spent preempted. The host response time includes release-to-completion delay.
