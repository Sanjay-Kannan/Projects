# Design decisions

- STM32H743-class target provides headroom for multi-channel acquisition and a 2048-point transform; this is a design target, not a measured requirement.
- FreeRTOS separates sampling, DSP, health decisions, communications, and logging so slow output cannot block acquisition.
- DMA and ping-pong buffers reduce per-sample CPU work and make ownership explicit.
- A 10 kHz vibration rate captures useful machinery harmonics below 5 kHz; anti-alias filtering is required in hardware.
- A 2048-point FFT at 10 kHz gives 4.88 Hz bin spacing over 204.8 ms, trading frequency detail against update latency.
- Deterministic signal models make regressions reproducible; simple threshold logic is explainable and inspectable.
- Sensor fusion combines independent modalities so a single noisy channel is less likely to dominate a decision.
- Host simulation exercises portable C logic before board access; no physical measurements are represented.
- A four-layer PCB is a reasonable conceptual choice for return paths, analog/digital separation, and dense MCU routing, but actual stackup depends on layout and EMC review.
