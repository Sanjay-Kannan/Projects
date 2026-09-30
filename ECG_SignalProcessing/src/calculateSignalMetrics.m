function m=calculateSignalMetrics(raw,processed,fs)
raw=double(raw(:));processed=double(processed(:));n=min(numel(raw),numel(processed));raw=raw(1:n);processed=processed(1:n);w=max(3,round(fs));
% SNR intentionally omitted without an independently measured clean reference.
m=struct('samples',n,'duration_s',n/fs,'raw_mean',mean(raw),'processed_mean',mean(processed),'raw_std',std(raw),'processed_std',std(processed),'raw_rms',sqrt(mean(raw.^2)),'processed_rms',sqrt(mean(processed.^2)),'raw_peak_to_peak',range(raw),'processed_peak_to_peak',range(processed),'residual_rms_proxy',sqrt(mean((raw-processed).^2)),'baseline_proxy_raw',std(movmean(raw,w)),'baseline_proxy_processed',std(movmean(processed,w)),'snr_db',NaN);
end
