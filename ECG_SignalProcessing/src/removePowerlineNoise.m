function y=removePowerlineNoise(x,fs,f0,Q)
[b,a]=designNotchFilter(fs,f0,Q);y=filtfilt(b,a,double(x(:)));
end
