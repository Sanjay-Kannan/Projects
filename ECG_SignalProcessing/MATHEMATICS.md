# Mathematical background

Let x(t) be a continuous voltage, x[n] its sampled values, fs samples/second, and n the integer sample index.

## Sampling and discrete time
`x[n]=x(n/fs)`. Nyquist frequency is `f_N=fs/2`. Frequencies above f_N alias into the sampled band without analog anti-alias filtering. Discrete-time angular frequency is `ω=2πf/fs` radians/sample.

## Convolution and filters
An LTI filter has `y[n]=Σ_k h[k]x[n-k]`, where h is its impulse response. FIR: `y[n]=Σ_{k=0}^{M} b_k x[n-k]`. IIR difference equation: `y[n]=Σ_{k=0}^{M}b_kx[n-k] - Σ_{k=1}^{N}a_ky[n-k]` (a0 normalized to 1). Coefficients determine magnitude and phase; feedback can destabilize an IIR if poles leave the unit circle.

## DFT and FFT spectrum
`X[k]=Σ_{n=0}^{N-1}x[n]e^{-j2πkn/N}`, k=0,...,N-1. FFT is an algorithm to evaluate the DFT efficiently. Frequency bin `f_k=k fs/NFFT`; bin spacing `Δf=fs/NFFT`. For a real signal, the positive-frequency amplitude estimate is `|X[k]|/N` for DC/Nyquist and `2|X[k]|/N` for other positive bins. N is record length; NFFT may be zero-padded to a convenient length. Zero padding interpolates the display, not the underlying information.

## RMS and SNR
`x_RMS=sqrt((1/N)Σx[n]^2)`. If independent signal and noise powers are known, `SNR_dB=10log10(P_signal/P_noise)`. A clean reference can support error-power estimation; without it, raw-filtered residual is not necessarily noise. Hence the code leaves SNR undefined.

## Beat intervals and HR
For R-peak positions p_i in samples, `RR_i=(p_{i+1}-p_i)/fs` seconds. `HR_i=60/RR_i` beats/minute. Sample positions must use the same indexing convention and time origin.

## Detection metrics
With one-to-one matched beats: `Sensitivity=TP/(TP+FN)`, `Precision=PPV=TP/(TP+FP)`, `F1=2·Precision·Sensitivity/(Precision+Sensitivity)`. TP is a matched pair within tolerance, FP is an unmatched detection, and FN is an unmatched reference. If denominators are zero, the implementation guards division; interpret such edge cases explicitly.
