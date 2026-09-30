function [b,a]=designBaselineFilter(fs,cutoff,order)
validateCutoff(fs,cutoff);[b,a]=butter(order,cutoff/(fs/2),'high');
end
