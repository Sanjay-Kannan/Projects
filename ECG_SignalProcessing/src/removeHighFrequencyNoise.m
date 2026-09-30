function y=removeHighFrequencyNoise(x,fs,cutoff,order)
[b,a]=designLowpassFilter(fs,cutoff,order);y=filtfilt(b,a,double(x(:)));
end
