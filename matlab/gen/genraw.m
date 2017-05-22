% Convert raw simulation data to mat files
initenv;
tic;

if env.full
    for iT = 1:env.nT
        disp(['T = ' env.Tstr{iT}]);
        for iv = 1:env.nv
            dt = env.dt(iT,iv);
            if dt < 0; continue; end
            disp(['v = ' env.vstr{iv}]);
            exvdir = fullfile(env.exdir, 'macro', ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            datavdir = fullfile(env.datadir,      ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            rtags = dir(fullfile(exvdir, 'r*')); rtags = {rtags.name};
            nrun = length(rtags);
            for irun = 1:nrun
                rtag = rtags{irun}; disp(rtag);
                datardir = fullfile(datavdir, rtag);
                mkdirx(datardir);
                trajfile = fullfile(exvdir, rtag, 'traj');
%                 efile = fullfile(exvdir, rtag, 'e');
                m = trajread(trajfile, dt);
%                 m.e = dlmread(efile);
                mfile = fullfile(datardir, 'm');
                save(mfile, 'm');
            end
        end
    end
end

toc;
