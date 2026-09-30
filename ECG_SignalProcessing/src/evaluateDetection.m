function m=evaluateDetection(detected,reference,fs,toleranceSec)
% Greedy one-to-one nearest matching: reference beats cannot be reused.
d=sort(detected(:));r=sort(reference(:));used=false(size(r));tp=0;tol=round(toleranceSec*fs);
for k=1:numel(d),ix=find(~used & abs(r-d(k))<=tol);if ~isempty(ix),[~,j]=min(abs(r(ix)-d(k)));used(ix(j))=true;tp=tp+1;end,end
fp=numel(d)-tp;fn=numel(r)-tp;sen=tp/max(1,tp+fn);ppv=tp/max(1,tp+fp);m=struct('TP',tp,'FP',fp,'FN',fn,'Sensitivity',sen,'Precision',ppv,'F1',2*sen*ppv/max(eps,sen+ppv),'tolerance_s',toleranceSec);
end
