function runExperiments(x,fs,referencePeaks,outputDir)
% Controlled parameter sweeps on supplied real ECG; injected tones/noise are
% perturbations of that recording and must not be reported as real observations.
if nargin<3,referencePeaks=[];end;if nargin<4,outputDir=fullfile(fileparts(fileparts(mfilename('fullpath'))),'results');end
if ~exist(outputDir,'dir'),mkdir(outputDir);end;x=double(x(:));base=struct('baselineCutoff',.5,'lineFrequency',60,'notchQ',35,'lowpassCutoff',40,'filterOrder',4,'integrationWindow',.15,'refractoryPeriod',.25,'thresholdFraction',.25,'matchTolerance',.15);
rows=[];cuts=[25 35 40];orders=[2 4 6];
for c=cuts
 for n=orders
  p=base;p.lowpassCutoff=c;p.filterOrder=n;[y,~,~]=preprocessECG(x,fs,p);[pk,~]=detectRPeaks(y,fs,p);row=table(c,n,numel(pk),'VariableNames',{'LowpassHz','Order','DetectedBeats'});
  if ~isempty(referencePeaks),q=evaluateDetection(pk,referencePeaks,fs,p.matchTolerance);row.TP=q.TP;row.FP=q.FP;row.FN=q.FN;row.Sensitivity=q.Sensitivity;row.Precision=q.Precision;row.F1=q.F1;end
  rows=[rows;row];
 end
end
writetable(rows,fullfile(outputDir,'experiment_filter_sweep.csv'));
% Threshold experiment varies detection threshold on unchanged processed input.
p=base;y=preprocessECG(x,fs,p);thresholds=[.10 .20 .25 .35 .50];tr=[];
for q=thresholds,p.thresholdFraction=q;pk=detectRPeaks(y,fs,p);r=table(q,numel(pk),'VariableNames',{'ThresholdFraction','DetectedBeats'});if ~isempty(referencePeaks),z=evaluateDetection(pk,referencePeaks,fs,p.matchTolerance);r.TP=z.TP;r.FP=z.FP;r.FN=z.FN;r.Sensitivity=z.Sensitivity;r.Precision=z.Precision;r.F1=z.F1;end;tr=[tr;r];end
writetable(tr,fullfile(outputDir,'experiment_threshold_sweep.csv'));
% Mains experiment injects a documented 60-Hz tone into real data and checks
% attenuation at the tone bin before/after notch. This is a controlled perturbation.
t=(0:numel(x)-1)'/fs;xn=x+0.05*std(x)*sin(2*pi*60*t);p.lineFrequency=60;z=removePowerlineNoise(xn,fs,60,35);[f,a]=computeFFT(xn,fs);[~,b]=computeFFT(z,fs);k=abs(f-60)<fs/numel(x);tone=table(60,mean(a(k)),mean(b(k)),'VariableNames',{'ToneHz','BeforeAmplitude','AfterAmplitude'});writetable(tone,fullfile(outputDir,'experiment_notch_tone.csv'));
end
