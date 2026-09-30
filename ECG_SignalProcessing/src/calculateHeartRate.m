function h=calculateHeartRate(peaks,fs)
if numel(peaks)<2,h=struct('time',[],'rr',[],'instantaneous',[],'mean',NaN,'minimum',NaN,'maximum',NaN);return;end
t=double(peaks(:))/fs;rr=diff(t);bpm=60./rr;h=struct('time',(t(1:end-1)+t(2:end))/2,'rr',rr,'instantaneous',bpm,'mean',mean(bpm),'minimum',min(bpm),'maximum',max(bpm));
end
