function [f,A]=computeFFT(x,fs,nfft)
% FFT-based single-sided amplitude spectrum with coherent normalization.
x=double(x(:));N=numel(x);if nargin<3||isempty(nfft),nfft=2^nextpow2(N);end
if nfft<N,error('nfft must be at least signal length');end
X=fft(x,nfft);A=abs(X(1:floor(nfft/2)+1))/N;if numel(A)>2,A(2:end-1)=2*A(2:end-1);end
f=(0:floor(nfft/2))'*fs/nfft;
end
