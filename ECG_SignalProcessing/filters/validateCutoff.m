function validateCutoff(fs,f)
validateattributes(fs,{'numeric'},{'scalar','finite','positive'});validateattributes(f,{'numeric'},{'scalar','finite','positive','<',fs/2});
end
