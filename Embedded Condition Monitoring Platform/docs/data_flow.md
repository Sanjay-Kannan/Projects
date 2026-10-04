# Data flow

High-rate ADC/DMA callbacks timestamp and publish completed fixed-size blocks. Each block has an owner, sequence number, and completion time. The processing task takes ownership, removes DC, windows and transforms the vibration block, then publishes features to monitoring. Slow temperature samples are held with their acquisition time and joined to the nearest health update.

The host simulation generates deterministic blocks through sensor drivers and exercises the same double-buffer ownership and DSP/monitor modules. The simulated DMA completion callback uses configured sample intervals for timestamps; it does not emulate bus electrical behavior or interrupt timing.
