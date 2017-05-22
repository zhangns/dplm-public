function [avg, err] = joinrun(runsdir, elementname, varargin)
% Average and error of ensemble runs
opt = getopt(struct('pattern', 'r*'), varargin{:});

rtags = dir(fullfile(runsdir, opt.pattern)); rtags = {rtags.name};
nrun = length(rtags);
for irun = 1:nrun
    element = dlmread(fullfile(runsdir, rtags{irun}, elementname));
    nrec(irun) = size(element, 1);
    elementlist{irun} = element;
end
nrec = min(nrec);
for irun = 1:nrun
    elements(:,:,irun) = elementlist{irun}(1:nrec,:);
end
avg = mean(elements(:,:,:), 3);
err = std(elements(:,:,:), 0, 3);

end
