initenv;
sisfdir = fullfile(env.resdir, 'sisf'); mkdirx(sisfdir);

% Axis limit
FsLim = [-0.02 1.02];

% Fs < FsEps is considered noise, not plotted or used
% The value is determined by stderr and examining the KWW plot visually
FsEps = 1e-3;

% To obtain KWW beta, fit with data points (t,Fs(t)) in KWW scale such that
% FsKWWmin <= Fs(t) <= FsKWWmax
% FsKWWmax is chosen so that the fit is reliable and accurate 
FsKWWmin = FsEps; FsKWWmax = 0.9;

% talpha is given by Fs(t=talpha) = Fstalpha
Fstalpha = exp(-1);

if env.full
    for iT = 1:env.nT
        iv = env.iv;
        % t{iT,iv} Fs{iT,iv,ik} Fserr{iT,iv,ik}
        exmivdir = fullfile(env.exdir, 'micro', ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
        datavdir = fullfile(env.datadir,        ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
        mkdirx(datavdir);
        for ik = 1:env.nk
            k02 = sprintf('%02d', env.k(ik));
            sisfk = ['sisf_' k02];
            [avg,err] = joinrun(exmivdir, sisfk);
            if env.dt(iT,iv) > 0
                [avgma,errma] = joinrun(datavdir, sisfk);
                [avg,err] = joinmode({avg, avgma}, {err, errma});
            end
            if ik == 1
                t{iT,iv} = avg(:,1);
            end
            Fs{iT,iv,ik} = avg(:,2);
            Fserr{iT,iv,ik} = err(:,2);
            dlmsave(fullfile(datavdir, sisfk), [t{iT,iv} Fs{iT,iv,ik} Fserr{iT,iv,ik}]);

            if min(Fs{iT,iv,ik}) > 0.3
                warning('Not relaxed with %g T %.3f v %.3f', min(Fs{iT,iv,ik}), env.T(iT), env.v(iv));
            else
                % talpha(iT,iv,ik)
                found = find(Fs{iT,iv,ik} > Fstalpha); it = found(end); it = [it, it+1];
                talpha(iT,iv,ik) = interp1(Fs{iT,iv,ik}(it), t{iT,iv}(it), Fstalpha);
                dlmsave(fullfile(datavdir, ['talpha_' k02]), talpha(iT,iv,ik));

                % beta(iT,iv,ik) mblt(iT,iv,ik)
                % mblt = -beta*log(talpha)
                msk = (Fs{iT,iv,ik} <= FsKWWmax) & ...
                      (Fs{iT,iv,ik} >= FsKWWmin);
                x = t{iT,iv}(msk); y = -log(Fs{iT,iv,ik}(msk));
                KWWmask{iT,iv} = msk;
                p = polyfit(log(x), log(y), 1);
                beta(iT,iv,ik) = p(1); mblt(iT,iv,ik) = p(2);
                dlmsave(fullfile(datavdir, ['KWW_' k02]), [beta(iT,iv,ik), mblt(iT,iv,ik)]);
            end
        end
    end
    % To compute D*talpha
    D = dlmread(fullfile(env.datadir, 'D'));

    for ik = 1:env.nk
        k = env.k(ik);
        k02 = sprintf('%02d', k);
        dlmsave(fullfile(env.datadir, ['talpha_' k02]), talpha(:,:,ik));
        dlmsave(fullfile(env.datadir, ['beta_' k02]), beta(:,:,ik));

        iv = env.iv;
        
        fignew('t', sprintf('F_s(q''=%d, t)', k), 'x');
%         title(sprintf('\\phi_v = %.3f', env.v(iv)));
        xlabel('$t$', 'Interpreter', 'Latex');
        ylabel(sprintf('$F_s(q''=%d, t)$', k), 'Interpreter', 'Latex');
        for iT = 1:env.nT
            if env.errbar
                errorbar(t{iT,iv}, Fs{iT,iv,ik}, Fserr{iT,iv,ik});
            else
                plot(t{iT,iv}, Fs{iT,iv,ik});
            end
        end
        xlim([1e-8 1e6]); ylim(FsLim);
        subnumber '(a)';
        figsave(fullfile(sisfdir, ['Fs_' k02]));

        fignew('t', sprintf('-log(F_s(q''=%d, t))', k), 'xy');
%         title(sprintf('\\phi_v = %.3f', env.v(iv)));
        xlabel('$t$', 'Interpreter', 'Latex');
        ylabel(sprintf('$-\\log(F_s(q''=%d, t))$', k), 'Interpreter', 'Latex');
        for iT = 1:env.nT
            msk = Fs{iT,iv,ik} > FsEps;
            x = t{iT,iv}(msk); y = -log(Fs{iT,iv,ik}(msk));
            if env.errbar
                h = errorbar(x, y, Fserr{iT,iv,ik}(msk));
            else
                h = plot(x, y);
            end
            if env.plotfit
                x = t{iT,iv}(KWWmask{iT,iv});
                plot(x, exp(beta(iT,iv,ik)*log(x)+mblt(iT,iv,ik)), ':k');
            end
        end
        xlim([1e-8 1e6]); ylim([1e-5 1e1]);
        subnumber '(b)';
        figsave(fullfile(sisfdir, ['FsKWW_' k02]));

%         fignew;
%         [hAx,hLine1,hLine2] = plotyy(env.invT, beta(:,iv,ik), env.invT, D(:,iv).*talpha(:,iv,ik));
%         xlabel 1/T;
%         ylabel(hAx(1), sprintf('\\beta(k''=%d)', k));
%         ylabel(hAx(2), sprintf('D\\tau_{\\alpha}(k''=%d)', k));
%         set(hLine1, 'Marker', '+');
%         set(hLine2, 'Marker', '+');
%         subnumber '(d)';
%         figsave(fullfile(sisfdir, ['betaDtalphaT_' k02]));
        
        fignew('1/T', sprintf('\\beta(q''=%d)', k));
        xlabel('$1/T$', 'Interpreter', 'Latex');
        ylabel(sprintf('$\\beta(q''=%d)$', k), 'Interpreter', 'Latex');
        plot(env.invT, beta(:,iv,ik), '+-r');
        ylim([0.8 1]);
        subnumber '(a)';
        figsave(fullfile(sisfdir, ['betaT_' k02]));
        
        fignew('1/T', sprintf('D\\tau_{\\alpha}(q''=%d)', k));
        xlabel('$1/T$', 'Interpreter', 'Latex');
        ylabel(sprintf('$D\\tau_{\\alpha}(q''=%d)$', k), 'Interpreter', 'Latex');
        plot(env.invT, D(:,iv).*talpha(:,iv,ik), '+-b');
        subnumber '(b)';
        figsave(fullfile(sisfdir, ['DtalphaT_' k02]));
        
        fignew('1/T', sprintf('\\tau_{\\alpha}(q''=%d)', k), 'y');
        plot(env.invT, talpha(:,iv,ik), '+-');
        figsave(fullfile(sisfdir, ['talphaT_' k02]));

%         % 3D
%         [T,V] = meshgrid(env.v, 1./env.T);
%         figure;
%         surf(V, T, beta(:,:,ik));
%         labels('1/T', '\phi_v', sprintf('\\beta(k''=%d)', k));
%         logscale('y');
%         savefig(fullfile(sisfdir, ['betaTv' k02]));
%         if strcmp(env.visible, 'off'); close; end
% 
%         figure;
%         surf(V, T, talpha(:,:,ik));
%         labels('1/T', '\phi_v', sprintf('\\tau_{\\alpha}(k''=%d)', k));
%         logscale('yz');
%         savefig(fullfile(sisfdir, ['talphaTv_' k02]));
%         if strcmp(env.visible, 'off'); close; end
% 
%         figure;
%         surf(V, T, D .* talpha(:,:,ik));
%         labels('1/T', '\phi_v', sprintf('D\\tau_{\\alpha}(k''=%d)', k));
%         logscale('y');
%         savefig(fullfile(sisfdir, ['DtalphaTv_' k02]));
%         if strcmp(env.visible, 'off'); close; end

    end
end
