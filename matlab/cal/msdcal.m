function [msd] = msdcal(m, varargin)
% Modified by Zhang Linghan

% calculate mean squared displacment g(dt) of atoms in selected dimensions (not normalized by no. of dimensions)
%   m = atomic coords object, from xtcread or (lmpread & unwrap)
% output: msd.t : dt
%         msd.g : g(dt)
%         msd.tau : relaxation time
%         msd.D : diffusion coeff
% output files: msd.txt msdtau.txt
% arguments:
%    dim : dimensions along which msd is calculated
%    interval : sampling interval
%    frame  : array of selected frames to calculate (0 = all frames)
%    fr1    : first frame to use (0 = use opt.frame)
%    nfr    : no. of frames to use (only used if fr1 used)
%    frinc  : fractional increament of dt for next data point
%    g0tau  : value of g considered relaxed, for calcuating tau
%    g0D    : value of g consdiered diffusive, for calculating D
%    COM    : if set, subtract center of mass motion
% e.g. msd = msdcal(m, 'interval', 10, 'frames', 1:1000)

opt = getopt( struct('dim', [1 2], 'interval', 10, 'frame', 0, 'frinc',1.4, 'g0tau',0.05, 'g0D',1, 'fr1',0, ...
                     'nfr',1000000, 'COM','noarg'), varargin{:});

ndim = size(opt.dim, 2);

if (opt.fr1 > 0)
    opt.frame = opt.fr1:(opt.fr1+opt.nfr-1);
elseif (opt.frame == 0)
    opt.frame = 1:m.nframe; % all frames
end;
framestart = opt.frame(1); % ref. time frame
frameend = min(m.nframe, opt.frame(end));

dfr=1;
dfrcnt=1;
while dfr< frameend-framestart % msdcal.11.m is parfor version, but even slower
    dt(dfrcnt) = m.dt*dfr;
    framesample = frameend-dfr : -opt.interval : framestart;
    r2mean1 = [];
    for frcnt = 1:length(framesample)
        fr = framesample(frcnt);
    	dr = squeeze( m.r(fr+dfr,:,:) - m.r(fr,:,:) ); % disp vector of all atoms
        if (size(dr,2)==1) dr=dr'; end; % needed for 1 particle case
        if (opt.COM) % substrate for Center of Mass
          drmean = mean(dr);
          dr = dr - ones(m.n,1)*drmean;
        end;
        dr2 = dot(dr(:,opt.dim),dr(:,opt.dim), 2);
        r2mean1(frcnt) = double(mean( dr2 )); % average over all atoms, in dim
    end;
    r2mean(dfrcnt) = mean(r2mean1); % average over all initial frames
    dfrcnt = dfrcnt+1;
    dfr = round(dfr*opt.frinc+1); % factor determines density of points in t axis
end;

msd.t = dt;     % t array
msd.g = r2mean; % msd array

% % plotting and save to file
% T = mean(m.T(framestart:frameend)); % temperature
% ctitle=num2str(T);
% figtitle(strcat('MSD'));
% data = [dt' r2mean'];
% save -ascii -append 'msd.txt' data;
% system('echo >> msd.txt');
% myplot('ll', 'lp', msd.t,msd.g, 'ctitle', ctitle );
% xlabel('t'); ylabel('g1');
% hold all;
%
% % find ralexation time tau and diffusion coeff D
% msd.tau = 0;
% ind = find( r2mean<opt.g0tau, 1, 'last' );
% if length(ind)>0 && ind<size(r2mean,2)
%   msd.tau = interp1( r2mean([ind ind+1]), dt([ind ind+1]), opt.g0tau );
% end;
% msd.D = 0;
% dto = dt(r2mean>opt.g0D); % points outside cage
% r2o = r2mean(r2mean>opt.g0D); % points outside cage
% if size(dto,2)>0
%   msd.D = mean(r2o./dto)/ndim;
% end;
% data = [T, msd.tau msd.D];
% save -ascii 'msdtau.txt' data;
%
% % plotting other auxillary lines
% dtb = [min(dt,0.1) 0.2];
% if (msd.t(1)<1) myplot('noinc', 'li', dtb,ndim*T*dtb.^2); end; % Ballistic at T
% myplot('noinc', 'li', 'ctitle','g0tau', msd.t([1 end]),opt.g0tau*([1 1]));
% myplot('noinc', 'li', 'ctitle','g0D', msd.t([1 end]),opt.g0D*([1 1]));
% myplot('noinc', 'li', 'ctitle','t^{0.25}', msd.t([1 end]),0.02*msd.t([1 end]).^0.25 ); % Reptation, Rouse-in-tube
% myplot('noinc', 'li', 'ctitle','t^{0.5}', msd.t([1 end]),0.03*msd.t([1 end]).^0.5 ); % Rouse
% myplot('noinc', 'li', 'ctitle','t^{1}', msd.t([1 end]),0.1*msd.t([1 end]).^1 ); % diffusion
% hold off;
