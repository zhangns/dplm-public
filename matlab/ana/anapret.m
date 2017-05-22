initenv;
pretdir = fullfile(env.resdir, 'pret'); mkdirx(pretdir);

if env.full
    for iT = 1:env.nT
        for iv = 1:env.nv
            % p1(iT,iv) p1err(iT,iv)
            % p2(iT,iv) p2err(iT,iv)
            exmivdir = fullfile(env.exdir, 'micro', ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            datavdir = fullfile(env.datadir,        ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            mkdirx(datavdir);
            [avg,err] = joinrun(exmivdir, 'pret');
            nhop = avg(3);
            p1(iT,iv)    = avg(1) / nhop;
            p2(iT,iv)    = avg(2) / nhop;
            p1err(iT,iv) = err(1) / nhop;
            p2err(iT,iv) = err(2) / nhop;
            dlmsave(fullfile(datavdir, 'pret'), [p1(iT,iv) p1err(iT,iv); p2(iT,iv) p2err(iT,iv)]);
        end
    end
    dlmsave(fullfile(env.datadir, 'p1'), p1);
    dlmsave(fullfile(env.datadir, 'p2'), p2);
    dlmsave(fullfile(env.datadir, 'p1err'), p1err);
    dlmsave(fullfile(env.datadir, 'p2err'), p2err);
    
    fignew('1/T', 'P');
    for iv = 1:env.nv
        if env.errbar
            errorbar(env.invT, p1(:,iv), p1err(:,iv), 'b');
            errorbar(env.invT, p2(:,iv), p2err(:,iv), 'r');
        else
            plot(env.invT, p1(:,iv), 'b');
            plot(env.invT, p2(:,iv), 'r');
        end
    end
    legend('P_{ret}', 'P_2', 'Location', 'northwest');
    figsave(fullfile(pretdir, 'pretT'));
    
    fignew('\phi_v', 'P');
    for iT = 1:env.nT
        if env.errbar
            errorbar(env.v, p1(iT,:), p1err(iT,:), 'b');
            errorbar(env.v, p2(iT,:), p2err(iT,:), 'r');
        else
            plot(env.v, p1(iT,:), 'b');
            plot(env.v, p2(iT,:), 'r');
        end
    end
    legend('P_{ret}', 'P_2', 'Location', 'northwest');
    figsave(fullfile(pretdir, 'pretv'));
    
    fignew('1/T', 'Probabilities');
    xlabel('$1/T$', 'Interpreter', 'Latex');
    iv = env.iv;
    if env.errbar
        errorbar(env.invT, p1(:,iv), p1err(:,iv), 'b');
        errorbar(env.invT, p2(:,iv), p2err(:,iv), 'r');
    else
        plot(env.invT, p1(:,iv), 'o-b', 'MarkerFaceColor', 'b');
        plot(env.invT, p2(:,iv), 's-r', 'MarkerFaceColor', 'r');
    end
    legend({'$P_{ret}$', '$P_2$'}, 'Interpreter', 'Latex', 'Location', 'northwest');
    figsave(fullfile(pretdir, 'pret'));
    
end
