function mkdirx(dirname)
% mkdir if not exists

if ~exist(dirname, 'dir')
    mkdir(dirname);
end

end
