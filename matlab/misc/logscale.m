function logscale(scale)
% Axis log scales

len = length(scale);

for i = 1:len
    a = scale(i);
    if strcmp(a, 'x')
        set(gca, 'XScale', 'log');
    elseif strcmp(a, 'y')
        set(gca, 'YScale', 'log');
    elseif strcmp(a, 'z')
        set(gca, 'ZScale', 'log');
    end
end

hold on;

end
