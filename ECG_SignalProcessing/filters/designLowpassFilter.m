function [b,a]=designLowpassFilter(fs,cutoff,order)
validateCutoff(fs,cutoff);[b,a]=butter(order,cutoff/(fs/2),'low');
end
