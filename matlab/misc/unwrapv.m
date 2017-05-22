function m = unwrapv(m_in)
% Unwrap periodic boundary conditions
% Modifed from unwrap.m by Zhang Linghan
% 19 Mar 2016
% Vectorize for better speed
% m should contain atom positions and box coords.

m = m_in;
L = m.rmax - m.rmin;
[tmax, natom, d] = size(m.r);
% for t = 2 : tmax
%     for n = 1 : natom
%         for k = 1 : d
%             while (m.r(t,n,k) - m.r(t-1,n,k) > L(k)/2)
%                 m.r(t,n,k) = m.r(t,n,k) - L(k);
%             end
%             while (m.r(t,n,k) - m.r(t-1,n,k) < -L(k)/2)
%                 m.r(t,n,k) = m.r(t,n,k) + L(k);
%             end
%         end
%     end
% end

for t = 2:tmax
    for k = 1:d
        dr = m.r(t,:,k) - m.r(t-1,:,k);
        m.r(t,:,k) = m.r(t,:,k) - L(k) * round(dr/L(k));
    end
end
