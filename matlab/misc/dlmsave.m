function dlmsave(filename, m, varargin)
% Wrapper for dlmwrite

global env;
dlmwrite(filename, m, 'delimiter', '\t', 'precision', env.precision);

end
