function labels(xlab, ylab, zlab)
% Axis labels

xlabel(xlab);
ylabel(ylab);

if nargin >= 3
    zlabel(zlab);
end

hold on;

end
