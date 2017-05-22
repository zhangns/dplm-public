function figsave(cname, w, h)
global env;

if nargin <= 2
    w = 5;
    h = 3;
end

set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPositionMode', 'manual');
set(gcf, 'PaperPosition', [0 0 w h]);
set(gcf, 'PaperSize', [w h]);

if env.savefig;
    savefig(cname);
end

if env.savepng
    print(cname, '-dpng', env.resolution);
end

if env.savepdf
%     print(cname, '-depsc');
    print(cname, '-dpdf');
end

end
