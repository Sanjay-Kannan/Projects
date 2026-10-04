#ifndef DSP_H
#define DSP_H
#include <stddef.h>
typedef struct { float rms, variance, peak, peak_to_peak, crest_factor, kurtosis; float dominant_hz, spectral_energy, centroid_hz, fundamental, second_harmonic, high_band_energy; } DspFeatures;
void dsp_process(const float *samples, size_t n, float fs, DspFeatures *out);
void dsp_process_configured(const float *samples,size_t n,float fs,float highpass_hz,DspFeatures *out);
#endif
