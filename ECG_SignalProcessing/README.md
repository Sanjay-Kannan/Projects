# ECG Signal Processing and Analysis Using MATLAB

This project provides a reproducible MATLAB workflow for offline ECG analysis. It expects a recording supplied by the user; see `data/README.md` for instructions to obtain MIT-BIH data and export a lead as CSV or MAT.

## Run

1. Obtain a real recording and optional beat references as described in `data/README.md`.
2. Place `record.csv` and (optionally) `reference_peaks.csv` in `data/`.
3. Review the configuration block at the top of `main.m`, especially line frequency (50/60 Hz), sampling rate, analysis window, and filter cutoffs.
4. In MATLAB, set the current folder to this directory and run `main`.

Outputs are saved under `results/`. A ten-second analysis window is configured to make plots readable; set `cfg.timeWindow=[]` for the entire record. `reference_peaks.csv` contains one-based sample positions, not seconds. Ensure references are shifted consistently when analyzing a cropped window.

## Methods and parameter rationale

The default baseline filter is a fourth-order Butterworth high-pass at 0.5 Hz. Lowering the cutoff preserves more slow ECG content, including ST-level changes, but leaves more baseline drift. Raising it removes more drift and may distort that same slow content. The notch targets the configured 50 or 60 Hz mains frequency; Q=35 gives a bandwidth of about f0/Q. A higher Q makes the notch narrower, but less tolerant of mains-frequency variation. The fourth-order low-pass at 40 Hz reduces high-frequency muscle and electronic noise while retaining most QRS detail. Lower cutoffs smooth the waveform more and can soften QRS edges; higher cutoffs leave more noise.

The project uses MATLAB's `butter`, `iirnotch`, `freqz`, `filtfilt`, and `fft` functions. Filter coefficients and intermediate signals are returned for inspection. `filtfilt` processes the signal forward and backward, so it avoids phase delay but is only suitable for offline analysis; it also increases the effective filter order and can affect the record edges. For peak detection, the code band-pass filters the ECG, differentiates and squares it, integrates energy over 150 ms, applies a global threshold, enforces a 250 ms refractory interval, and refines each event against the waveform. This is a compact implementation for study, not the full adaptive Pan–Tompkins detector. Adjust the threshold and compare detections with annotations before drawing conclusions.

The global threshold is easy to explain but is sensitive to outliers and changing signal amplitude. Decreasing it raises sensitivity and false alarms; increasing it reduces false alarms but can miss smaller beats. The integration window should span the QRS energy envelope (150 ms here); longer windows merge nearby beats, shorter windows produce noisier energy. The refractory interval excludes physiologically implausible duplicate detections; excessively long intervals can suppress genuine fast beats. Sampling rate sets Nyquist frequency fs/2 and FFT bin spacing fs/NFFT; use the actual header value. A wrong fs corrupts every time/frequency result.

## Dependencies

MATLAB R2020b or later recommended. Signal Processing Toolbox is required for Butterworth/notch design, zero-phase filtering, and frequency response (`butter`, `iirnotch`, `filtfilt`, `freqz`). MATLAB base functionality supplies file I/O, FFT, tables, figures. No Statistics/Machine Learning Toolbox is required. WFDB command-line utilities or WFDB Toolbox are optional for preparing MIT-BIH data and annotations. The project itself does not download data.

## Results integrity

Figures and metrics are created when the project is run on a recording. SNR is left undefined because there is no independent clean reference. `residual_rms_proxy` is the RMS difference between raw and processed signals; it is not a direct estimate of noise power. Sensitivity, precision, and F1 are calculated only when an aligned reference CSV is provided. Scores apply to that recording and configuration.

## Project contents

`main.m` runs the pipeline. `src/` holds reusable functions, `filters/` holds coefficient designs, `data/` holds acquisition instructions, `report/project_report.md` is the report, and the two study documents cover interview concepts and mathematics.
