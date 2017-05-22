clear; clear global;
close all;
global env;

env.fullstr = 'fullprod';
env.exhaustive = 0;
env.visible = 'off';
env.plotfit = 0; % whether plot fitted lines when unnecessary
env.errbar = 0;
env.resolution = sprintf('-r%d', 400);
env.savefig = 0;
env.savepng = 1;
env.savepdf = 0;
env.savepdf = ~env.exhaustive;
env.precision = 6;

%{
glass
  ws
    full
      data(macro txt data)
      result: msd sisf fpcf
  ex
    full: micro(txt data) macro
%}
env.full = 1; env.study = env.fullstr;
env.basedir = fullfile(pwd, '..', '..');
env.exdir = fullfile(env.basedir, 'ex', env.study); % Runs
env.wsdir = fullfile(env.basedir, 'ws', env.study); % Workspace
env.datadir = fullfile(env.wsdir, 'data'); mkdirx(env.datadir); % Data
env.resdir = fullfile(env.wsdir, 'result'); mkdirx(env.resdir); % Result

env.w0 = 1e6;
env.EB0 = 1.5;
env.Vmin = -0.5;
env.Tstr = strsplit(fileread(fullfile(env.datadir, 'T')));
env.vstr = strsplit(fileread(fullfile(env.datadir, 'v')));
env.T = cellfun(@str2double, env.Tstr) / 1000;
env.v = cellfun(@str2double, env.vstr) / 1000;
env.nT = length(env.T);
env.invT = 1 ./ env.T;
env.k = dlmread(fullfile(env.datadir, 'k'));
env.nk = length(env.k);
env.q = dlmread(fullfile(env.datadir, 'q'));
assert(env.q(1) == 0);
env.nq = length(env.q);
env.dt = dlmread(fullfile(env.datadir, 'dt'))'; % One T is one column
% env.dt(:) = -1; % ignore macromode data

if env.full
    env.nv = length(env.v);
    assert(all(size(env.dt) == [env.nT env.nv]));
    % fix phi_v and change T
    env.iv = 4; % phi_v = 0.01
end
