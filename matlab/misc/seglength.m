function l = seglength(filename)
% Segment length of given file

fid = fopen(filename);
c = textscan(fgetl(fid), '%f');
l = 1;
while fgetl(fid)
    l = l + 1;
end
fclose(fid);

end
