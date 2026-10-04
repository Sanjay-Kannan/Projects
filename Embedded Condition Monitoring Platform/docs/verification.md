# Verification

Run `python scripts/run_all_tests.py`. Tests compile the C pipeline, exercise five deterministic scenarios, verify driver lifecycle/range handling, DMA-buffer ownership, filter response, CRC corruption/truncation, and shaft-rate FFT behavior, then regenerate CSV data. This is host-side functional verification only. It does not validate STM32 HAL bindings, FreeRTOS scheduling on target, MCU timing, analog conditioning, or physical fault sensitivity.
