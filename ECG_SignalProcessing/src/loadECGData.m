function [x,fs,meta]=loadECGData(file,fsDefault,channel)
if ~isfile(file),error('ECG:MissingFile','File not found: %s',file);end
[~,~,ext]=fileparts(file);meta=struct('name',file,'units','mV');
switch lower(ext)
 case '.csv'
  A=readmatrix(file);A=A(:,all(isfinite(A),1));if isempty(A)||channel>size(A,2),error('ECG:Channel','Requested numeric channel unavailable.');end;x=A(:,channel);fs=fsDefault;
 case '.mat'
  S=load(file);names=fieldnames(S);x=[];fs=fsDefault;
  for k=1:numel(names),v=S.(names{k});if isnumeric(v)&&isvector(v),x=v(:);break;end,end
  if isempty(x),error('ECG:MAT','MAT file must contain a numeric vector.');end
  if isfield(S,'fs'),fs=double(S.fs);end
 otherwise,error('ECG:Format','Use CSV/MAT; see data/README.md for WFDB export.');
end
x=double(x(:));if any(~isfinite(x)),error('ECG:NaN','Signal contains nonfinite samples.');end
end
