function [m] = trajread(filename, dt, ftrunc)
% Create m from traj file
% ftrunc (=1) is usually not needed

if nargin < 3
    ftrunc = 1;
end

m.dt = dt;
fid = fopen(filename);
% Determine d and n
c = textscan(fgetl(fid), '%f');
m.d = length(c{1});
m.n = 1;
while fgetl(fid)
    m.n = m.n + 1;
end
frewind(fid);
if ftrunc < 1
    % Seek to ftrunc before EOF
    finfo = dir(filename);
    fseek(fid, -int32(finfo.bytes * ftrunc), 'eof');
    if isempty(fgetl(fid))
        fseek(fid, -2, 'cof');
    end
    % Seek the next frame
    while fgetl(fid)
    end
    r = fscanf(fid, '%f', [m.d Inf]);
else
    % dlmread is faster than fscanf
    r = dlmread(filename)';
end
fclose(fid);

sz = size(r);
m.nframe = sz(2) / m.n;
m.r = permute(reshape(r, [m.d m.n m.nframe]), [3 2 1]);

m.L = max(m.r(1,:,1)) + 1; % Lattice side length
m.rmin = ones(m.d, 1) * (-0.5);
m.rmax = ones(m.d, 1) * (m.L - 0.5);
% plot(m.r(1:200:end,:,1));

end
