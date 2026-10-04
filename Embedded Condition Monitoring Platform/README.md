# Real-Time Multi-Sensor Edge Computing and Condition Monitoring Platform

## Overview

A portable C signal-processing and condition-monitoring core designed for an STM32H743-class rotating-machine monitor. It includes a deterministic host simulation so the pipeline can be built and exercised without a board. All sensor values are synthetic; no physical hardware measurements are claimed.

## Features

- C11 DSP core with 2048-point radix-2 FFT and time/frequency features.
- Five repeatable operating scenarios and CSV dataset generation.
- Four sensor drivers with simulation backends and an STM32 HAL adapter contract.
- DMA-style ping-pong buffers, ownership transfer, overflow counters, and configurable DC blocking.
- Optional FreeRTOS task wiring under `firmware/App/rtos` for target builds.
- Explainable threshold-based fault indications and health state.
- Compact CRC-protected UART frame codec.
- CMake host build and direct GCC build used by Python runner.
- GitHub Actions host verification.

## System Architecture

See [architecture](docs/architecture.md), [data flow](docs/data_flow.md), and [conceptual PCB](docs/pcb_architecture.md). The intended layers are Application → Middleware → BSP/HAL → STM32 HAL. Simulation replaces only the sensor/BSP source.

## Data Flow

Sensor interfaces → timestamped DMA-style blocks → bounded ping-pong buffering → filtering and FFT → features → condition monitor → UART telemetry and host visualization. The host executable currently runs the portable C DSP and classifier; target-specific HAL and FreeRTOS integration remain a porting step.

## Hardware Architecture

The design targets ADXL355-class vibration, TMP117-class temperature, an analog current interface, and an analog MEMS microphone. These devices are conceptual only and are not connected by the simulation. Sampling targets are 10 kHz vibration, 16 kHz acoustic, 2 kHz current, and 10 Hz temperature.

## Firmware Architecture

Firmware modules are organized under Core and Middleware. On target, use FreeRTOS tasks for acquisition, processing, monitoring, communications, and logging. DMA completion should publish buffer descriptors via task notification/queue; callbacks should do bounded work only.

## RTOS Architecture

The host build omits FreeRTOS to keep cloning and execution straightforward. Optional target task wiring uses bounded queues between acquisition, DSP, monitoring, UART, and logging tasks; an event group reports task errors and a mutex protects log output. A vendor-generated STM32 project and application callback implementations are still needed for a board build.

## Signal Processing

The C core calculates RMS, variance, peak, peak-to-peak, crest factor, kurtosis, dominant frequency, spectral energy, centroid, harmonic amplitudes, and high-band energy. The 2048-point frame spans 204.8 ms at 10 kHz, with 4.88 Hz bin spacing.

## Fault Detection

Threshold rules combine vibration, harmonic, acoustic, current, and temperature indicators. Detection strength is an indicator, not a statistical confidence. Initial thresholds require application-specific validation.

## Simulation

`python scripts/run_simulation.py` compiles and runs the C core, prints mean host CPU time averaged across 129 FFT passes, the simulated processing deadline, and observed double-buffer occupancy, then round-trips a UART frame and writes results. The two-block workload deliberately overlaps processing ownership with the next DMA completion, so both ping-pong slots are occupied. This is a modeled overlap case, not an MCU timing or worst-case load result. Install `requirements.txt` for plots. `python scripts/generate_dataset.py` writes deterministic scenario CSVs.

## Verification

Run `python scripts/run_all_tests.py`. It builds the host executable, exercises all five scenarios, checks shaft-rate FFT behavior, and generates datasets. See [verification](docs/verification.md).

## Example Results

Results are generated locally in `results/`; scenario outcome and features are printed by the simulation. Exact values are reproducible for a given compiler and configuration.

## Repository Structure

`firmware/` contains the C application and portable middleware; `host/` contains a lightweight plot viewer; `simulation/` contains generated CSV output; `scripts/` runs builds, datasets, and checks; `tests/` contains deterministic pipeline tests; `docs/` records design and limitations.

## How to Run

Prerequisites: GCC, Python 3.10+, and optionally CMake. From the repository root:

```sh
python scripts/run_simulation.py
python scripts/run_simulation.py --scenario BEARING_FAULT
python scripts/generate_dataset.py
python host/dashboard/dashboard.py NORMAL
```

## Testing

```sh
python scripts/run_all_tests.py
```

The host runner uses GCC directly; CMake is available for IDE integration: `cmake -S . -B build && cmake --build build`.

## Design Decisions

See [design decisions](docs/design_decisions.md) for sampling, FFT, DMA, RTOS, simulation, and PCB rationale.

## Limitations

The repository does not contain a complete STM32Cube/HAL port, concrete sensor bus drivers, or calibrated fault thresholds. FreeRTOS task wiring is target-facing but the target callback implementations and board build are not included. Host timing is not MCU timing. Analog anti-alias design and physical validation remain required before deployment.

## Future Hardware Validation

Port the BSP to an STM32H743 board, capture DMA timing and queue behavior, characterize each sensor and analog path, then validate thresholds against representative machine data.
