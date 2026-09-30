function y=removeBaselineWander(x,fs,cutoff,order)
[b,a]=designBaselineFilter(fs,cutoff,order);y=filtfilt(b,a,double(x(:)));
end
