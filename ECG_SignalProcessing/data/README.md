# ECG data

This folder includes record 100 from the [PhysioNet MIT-BIH Arrhythmia Database](https://physionet.org/content/mitdb/1.0.0/). MIT-BIH contains two-channel ambulatory ECG recordings sampled at 360 Hz with expert beat annotations. Record 100 is about 30 minutes long; its source header gives 650,000 samples per channel.

## Files provided

- `100.hea`, `100.dat`, `100.atr`: original PhysioNet header, signal, and beat-annotation files for record 100.
- `record.csv`: both channels, converted to millivolts and saved without a header. Column 1 is MLII; column 2 is V5.
- `reference_peaks.csv`: reference beat locations as one-based sample indices, in the same sample coordinate system as `record.csv`.

The CSV conversion unpacks the database's format-212 samples and applies the gain and baseline stated in `100.hea`. The reference list uses the standard beat annotation codes and excludes non-beat annotations. The source files are kept alongside the converted files so the conversion can be checked or repeated with WFDB tools.

## Run the project

The supplied `main.m` configuration defaults to record 100, channel 1, 360 Hz, and a ten-second analysis window. Set `cfg.channel=2` to use V5, or set `cfg.timeWindow=[]` to analyze the complete recording. Reference positions are automatically shifted to match a selected time window.

The database's recordings were digitized from ambulatory ECGs; they are not a clean laboratory reference. Therefore, the project does not calculate SNR against a fabricated clean signal. Cite the database and its original paper when using these data in a report or CV project:

1. PhysioNet, [MIT-BIH Arrhythmia Database](https://physionet.org/content/mitdb/1.0.0/).
2. G. B. Moody and R. G. Mark, “The impact of the MIT-BIH Arrhythmia Database,” *IEEE Engineering in Medicine and Biology Magazine*, vol. 20, no. 3, pp. 45–50, 2001.

To obtain another record, download its `.hea`, `.dat`, and `.atr` files from PhysioNet. WFDB tools such as `rdsamp` and `rdann` can be used to export another signal and its annotations; keep the channel, sample rate, time window, and one-based index convention consistent when creating replacement CSV files.
