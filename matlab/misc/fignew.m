function fignew(xlab, ylab, scale)
global env;

figure('Visible', env.visible);
hold on;
box on;

if nargin >= 2
    labels(xlab, ylab);
end

if nargin >= 3
    logscale(scale)
end

end
