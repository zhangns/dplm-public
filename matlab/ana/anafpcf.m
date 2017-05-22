initenv;
fpcfdir = fullfile(env.resdir, 'fpcf'); mkdirx(fpcfdir);

if env.full
    for iT = 1:env.nT
        iv = env.iv;
        % t{iT,iv} S4{iT,iv,ik,iq} S4err{iT,iv,ik,iq}
        exmivdir = fullfile(env.exdir, 'micro', ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
        datavdir = fullfile(env.datadir,        ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
        mkdirx(datavdir);
        for ik = 1:env.nk
            k02 = sprintf('%02d', env.k(ik));
            for iq = 1:env.nq
                q02 = sprintf('%02d', env.q(iq));
                fpcfkq = ['fpcf_' k02 '_' q02];
                [avg,err] = joinrun(exmivdir, fpcfkq);
                if env.dt(iT,iv) > 0
                    [avgma,errma] = joinrun(datavdir, fpcfkq);
                    [avg,err] = joinmode({avg, avgma}, {err, errma});
                end
                if ik == 1
                    t{iT,iv} = avg(:,1);
                end
                S4{iT,iv,ik,iq} = avg(:,2);
                S4err{iT,iv,ik,iq} = err(:,2);
                dlmsave(fullfile(datavdir, fpcfkq), [t{iT,iv} S4{iT,iv,ik,iq} S4err{iT,iv,ik,iq}]);
            end
        end
    end

    for ik = 1:env.nk
        k = env.k(ik);
        k02 = sprintf('%02d', k);
%         if env.nq > 1
%             % To compute S4(q>0, t=talpha)
%             talpha(:,:,ik) = dlmread(fullfile(env.datadir, ['talpha_' k02]));
%         end
        for iq = 1:env.nq
            q = env.q(iq);
            q02 = sprintf('%02d', q);
            if q == 0
                iv = env.iv;
                fignew('t', sprintf('\\chi_4(k''=%d, t)', k), 'x');
                xlabel('$t$', 'Interpreter', 'Latex');
                ylabel(sprintf('$\\chi_4(q''=%d, t)$', k), 'Interpreter', 'Latex');
                for iT = 1:env.nT
                    if env.errbar
                        errorbar(t{iT,iv}, S4{iT,iv,ik,iq}, S4err{iT,iv,ik,iq});
                    else
                        plot(t{iT,iv}, S4{iT,iv,ik,iq});
                    end
                end
                xlim([1e-4 1e+6]);
                figsave(fullfile(fpcfdir, ['chi4_' k02]));

%                 fignew('t', sprintf('\\chi_4(k''=%d, t)', k), 'xy');
%                 for iT = 1:env.nT
%                     if env.errbar
%                         errorbar(t{iT,iv}, S4{iT,iv,ik,iq}, S4err{iT,iv,ik,iq});
%                     else
%                         plot(t{iT,iv}, S4{iT,iv,ik,iq});
%                     end
%                 end                
%                 figsave(fullfile(fpcfdir, ['chi4l_' k02]));
            else % q > 0
                % TODO
            end
        end
    end
end
