function [loc,d]=detectRPeaks(x,fs,p)
% Pan-Tompkins-inspired QRS energy detector with separate visible stages.
x=double(x(:));[b,a]=butter(2,[5 min(20,fs/2-1)]/(fs/2),'bandpass');d.bandpass=filtfilt(b,a,x);
d.derivative=[0;diff(d.bandpass)]*fs;d.squared=d.derivative.^2;w=max(1,round(p.integrationWindow*fs));d.integrated=filter(ones(w,1)/w,1,d.squared);
thr=p.thresholdFraction*max(d.integrated);ix=find(d.integrated>thr);if isempty(ix),loc=[];return;end
cuts=[0;find(diff(ix)>1);numel(ix)];cand=zeros(numel(cuts)-1,1);for k=1:numel(cand),g=ix(cuts(k)+1:cuts(k+1));[~,j]=max(d.integrated(g));cand(k)=g(j);end
refr=round(p.refractoryPeriod*fs);keep=[];for k=1:numel(cand),if isempty(keep)||cand(k)-keep(end)>refr,keep(end+1)=cand(k);elseif d.integrated(cand(k))>d.integrated(keep(end)),keep(end)=cand(k);end,end
loc=zeros(size(keep));radius=round(.1*fs);for k=1:numel(keep),lo=max(1,keep(k)-radius);hi=min(numel(x),keep(k)+radius);[~,j]=max(abs(d.bandpass(lo:hi)));loc(k)=lo+j-1;end
loc=unique(loc(:));d.threshold=thr;d.windowSamples=w;
end
