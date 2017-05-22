% Generate MSD result
initenv;
tic;

if env.full
    for iT = 1:env.nT
        disp(['T = ' env.Tstr{iT}]);
        for iv = 1:env.nv
            dt = env.dt(iT,iv);
            if dt < 0; continue; end
            disp(['v = ' env.vstr{iv}]);
            datavdir = fullfile(env.datadir, ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            rtags = dir(fullfile(datavdir, 'r*')); rtags = {rtags.name};
            nrun = length(rtags);
            for irun = 1:nrun
                rtag = rtags{irun}; disp(rtag);
                datardir = fullfile(datavdir, rtag);
                mfile = fullfile(datardir, 'm');
                load(mfile); %m

                m = unwrapv(m);
                msd = msdcal(m);
                msdfile = fullfile(datardir, 'msd');
                dlmsave(msdfile, [msd.t; msd.g]');

            end
        end
    end
end

toc;
