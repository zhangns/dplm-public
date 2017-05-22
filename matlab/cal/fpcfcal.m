function [fpcf] = fpcfcal(m, varargin)
% Calculate sisf (self-intermediate scattering function) and
%           fpcf (four-point correlation function)

% Output:
%   fpcf.t, fpcf.Fs(ik,idt), fpcf.S4(iq,ik,idt)
% Arguments:
%   k        : reduced wave vector for mobility
%   q        : reduced wave vector for Fourier
%   interval : sampling interval
%   frinc    : fractional increament of dt

opt = getopt(struct('k', [10], ...
                    'q', [0], ...
                    'interval', 10, ...
                    'frinc', 1.4), varargin{:});

k = 2 * pi / m.L * opt.k(:); nk = length(k);
q = 2 * pi / m.L * opt.q(:); nq = length(q);

dfx = 1; idt = 1;
while dfx < m.nframe
    df(idt) = dfx;
    fpcf.t(idt) = m.dt * dfx;
    idt = idt + 1;
    dfx = round(dfx * opt.frinc + 1);
end
ndt = length(df);

% Fs
for idt = 1:ndt
    frsamp = m.nframe-df(idt) : -opt.interval : 1;
    dr = m.r(frsamp+df(idt),:,:) - m.r(frsamp,:,:);
    fpcf.Fs(:,idt) = mean(cos(k*dr(:)'), 2);
end

% S4
for idt = 1:ndt
    frsamp = m.nframe-df(idt) : -opt.interval : 1;
    nsamp = length(frsamp);
    dr = m.r(frsamp+df(idt),:,:) - m.r(frsamp,:,:);
    for ik = 1:nk
        for iq = 1:nq
            s4 = zeros(nsamp, m.n);
            for d1 = 1:2
                for d2 = 1:2
                    s4 = s4 + ...
                         cos(q(iq) * m.r(frsamp,:,d1)) .* ...
                         (cos(k(ik) * dr(:,:,d2)) - fpcf.Fs(ik,idt));
                end
            end
            fpcf.S4(iq,ik,idt) = mean((sum(s4/4, 2).^2)) / m.n;
        end
    end
end

end
