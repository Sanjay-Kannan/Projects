function [y,s,filt]=preprocessECG(x,fs,p)
% Offline zero-phase chain; each stage is returned for independent inspection.
x=double(x(:));[b1,a1]=designBaselineFilter(fs,p.baselineCutoff,p.filterOrder);s.baseline=filtfilt(b1,a1,x);
[b2,a2]=designNotchFilter(fs,p.lineFrequency,p.notchQ);s.notch=filtfilt(b2,a2,s.baseline);
[b3,a3]=designLowpassFilter(fs,p.lowpassCutoff,p.filterOrder);y=filtfilt(b3,a3,s.notch);s.final=y;
filt=struct('name',{'Baseline high-pass','Power-line notch','Low-pass'},'b',{b1,b2,b3},'a',{a1,a2,a3});
end
