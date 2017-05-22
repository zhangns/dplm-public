% Generate SISF & FPCF result
initenv;
tic;

if env.full
    iv = env.iv; % consider single phi_v
    disp(['v = ' env.vstr{iv}]);
    for iT = 1:env.nT
        disp(['T = ' env.Tstr{iT}]);
        dt = env.dt(iT,iv);
        if dt < 0; continue; end
        datavdir = fullfile(env.datadir, ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
        rtags = dir(fullfile(datavdir, 'r*')); rtags = {rtags.name};
        nrun = length(rtags);
        for irun = 1:nrun
            rtag = rtags{irun}; disp(rtag);
            datardir = fullfile(datavdir, rtag);
            mfile = fullfile(datardir, 'm');
            load(mfile); %m

            fpcf = fpcfcal(m, 'k', env.k, 'q', env.q);
            for ik = 1:env.nk
                k = env.k(ik);
                k02 = sprintf('%02d', k);
                sisffile = fullfile(datardir, ['sisf_' k02]);
                dlmsave(sisffile, [fpcf.t; fpcf.Fs(ik,:)]');
                for iq = 1:env.nq
                    q = env.q(iq);
                    q02 = sprintf('%02d', q);
                    fpcffile = fullfile(datardir, ['fpcf_' k02 '_' q02]);
                    dlmsave(fpcffile, [fpcf.t; squeeze(fpcf.S4(iq,ik,:))']');
                end
            end

        end
    end
end

toc;
