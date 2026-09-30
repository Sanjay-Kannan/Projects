function [b,a]=designNotchFilter(fs,f0,Q)
validateCutoff(fs,f0);wo=f0/(fs/2);[b,a]=iirnotch(wo,wo/Q);
end
