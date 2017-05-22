function [avg, err] = joinmode(avgs, errs)
% Join same element for micromode and macromode using dt
% avgs and errs are cells of nmode of matrix of [nt nrec]
% Prefer long mode if overlap in dt

nmode = length(avgs); assert(length(errs) == nmode);
avg = []; err = [];
dt = inf;

for imode = nmode:-1:1
    shortmask = avgs{imode}(:,1) < dt;
    avg = [avgs{imode}(shortmask,:) ; avg];
    err = [errs{imode}(shortmask,:) ; err];
    dt = avg(1,1);
end

end
