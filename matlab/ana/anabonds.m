% anabonds('C:/data/bondsdist42', 20, 20, 0.17, 0.01, 1000, 1);
function anabonds(bondsdatadir, n_randl, n_rands, T, v, dtl, dts)
% Standalone function for bonds distro study
% Do not run initenv
%
% Input data: bondsdatadir/r*/[e|BONDS]
%
% Output figures:
% (1) bondsdatadir/bonds_e.png : E vs t
% (2) bondsdatadir/bonds_p.png : p_t (ens avg) vs E for various t

n_rand = n_randl+ n_rands;
BONDS = 'bonds.2';
% Determine V (# of sites)
V = seglength(fullfile(bondsdatadir, 'r01', BONDS));
assert(V == 10000);

% X : bond energy, i.i.d. random variable
xmin = -0.5; xmax = 0.5;
Z = T * (exp(-xmin/T) - exp(-xmax/T)); % partition function
xbar = (exp(-xmin/T)*T*(T+xmin) - exp(-xmax/T)*T*(T+xmax)) / Z; % mean x

% if 0
% Figure 1
for irun = [1 2 n_rand+[3 8]] % 2 vs 2
    rtag = sprintf('r%02d', irun); 
    efile = fullfile(bondsdatadir, rtag, 'e');
    e = dlmread(efile);
    t = 1 : 500; % t <= 5e5
    if irun <= n_randl
        linestyle = '-';
    else
        linestyle = '--';
    end
    plot(dtl * t, e(t + 1), linestyle);
    hold on;
end
% Theoretical EQ value
phi = 1 - v;
xref = [0, t(end)*dtl];
yref = xbar * (4-2/phi); % 1st approximation
plot(xref, [yref yref], ':k');
xlim(xref);
xlabel('$t$', 'Interpreter', 'Latex');
ylabel('$E/N$', 'Interpreter', 'Latex');
subnumber '(a)'
% Save figure
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPositionMode', 'manual');
w = 5; h = 3;
set(gcf, 'PaperPosition', [0 0 w h]);
set(gcf, 'PaperSize', [w h]);
cname = fullfile(bondsdatadir, 'bonds_e');
print(cname, '-dpng', sprintf('-r%d', 400));
print(cname, '-dpdf');
% end

% Figure 2
nbins = 25;
x_edges = linspace(xmin, xmax, nbins + 1);
dx = (xmax - xmin) / nbins;
tl = [1 4 16 64 256 1000]; ntl = length(tl);
ts = [0 1 4 16 64 256];   nts = length(ts);
nt = ntl + nts;
Count = zeros(nt, nbins); % Count(it,ix)

% Read BONDS file for each run
for irun = 1 : n_rand
    rtag = sprintf('r%02d', irun);
    bondsfile = fullfile(bondsdatadir, rtag, BONDS);
    bonds = dlmread(bondsfile)';
    bonds = reshape(bonds, 2*V, [])'; % ntx20000
    if irun <= n_randl
        imin = nts + 1; imax = nts + ntl;
    else
        imin = 1; imax = nts;
    end
    for it = imin : imax
        bonds_t = bonds(it-imin+1,:);
        bonds_t = bonds_t(bonds_t > -100); % filter out dummy bonds
        [counts, ~] = histcounts(bonds_t, x_edges);
        Count(it,:) = Count(it,:) + counts;
    end
end
P = zeros(nt, nbins); % P(it,ix)
for it = 1 : nt
    P(it,:) = Count(it,:) / sum(Count(it,:)) / dx; 
end

figure;
x = (x_edges(2:end) + x_edges(1:end-1)) / 2;
for it = 1 : nt
    semilogy(x, P(it,:), '.-', 'MarkerSize', 7);
    if it <= nts
        legends{it} = sprintf('$t=%d$', dts * ts(it));
    else
        legends{it} = sprintf('$t=%d$', dtl * tl(it - nts));
    end
    hold on
end
xref = [xmin xmax];
yref = exp(-xref/T)/Z;
semilogy(xref, yref, '--k');
xlabel('$V_{ijs_i s_j}$'       , 'Interpreter', 'LaTex');
ylabel('$p_{t}(V_{ijs_i s_j})$', 'Interpreter', 'LaTex');
legend(legends, 'Location', 'best', 'FontSize', 8, 'Interpreter', 'LaTex');
subnumber '(b)'
% Save figure
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPositionMode', 'manual');
w = 5; h = 3.5;
set(gcf, 'PaperPosition', [0 0 w h]);
set(gcf, 'PaperSize', [w h]);
cname = fullfile(bondsdatadir, 'bonds_p');
print(cname, '-dpng', sprintf('-r%d', 400));
print(cname, '-dpdf');


% legend('Data', 'Theory', 'Location', 'northeast');
% axes('Position', [.2 .2 .3 .3]);
% box on
% plot(x, (y-ytheo)./ytheo, '+-');
% xlabel V
% ylabel \xi
% title 'Fractional deviation'

end
