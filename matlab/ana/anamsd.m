initenv;
msddir = fullfile(env.resdir, 'msd'); mkdirx(msddir);

% gLabel = 'g(t)';
gLabel = 'MSD';

% To obtain D, fit with data points (t,g(t)) such that 
% g(t) >= Dg, and
% In loglog plot, slope between this point to longest-t point >= Dslope
Dg = 1.0;
Dslope = 0.96;
DminNumPoint = 10; % Warn if too few points

% To obtain alpha, fit with data points (phi_v, D(phi_v)) such that
% phi_v <= FACvmax
FACvMax = 0.050;

if env.full
    for iT = 1:env.nT
        for iv = 1:env.nv
            % t{iT,iv} g{iT,iv} gerr{iT,iv}
            exmivdir = fullfile(env.exdir, 'micro', ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            datavdir = fullfile(env.datadir,        ['T' env.Tstr{iT}], ['v' env.vstr{iv}]);
            mkdirx(datavdir);
            [avg,err] = joinrun(exmivdir, 'msd');
            if env.dt(iT,iv) > 0
                [avgma,errma] = joinrun(datavdir, 'msd');
                [avg,err] = joinmode({avg, avgma}, {err, errma});
            end
            t{iT,iv} = avg(:,1);
            g{iT,iv} = avg(:,2);
            gerr{iT,iv} = err(:,2);
            dlmsave(fullfile(datavdir, 'msd'), [t{iT,iv} g{iT,iv} gerr{iT,iv}]);
            
            % D(iT,iv)
            tx = t{iT,iv}; gx = g{iT,iv}; 
            msk = (gx >= Dg) & ...
                  (log(gx(end)./gx)./log(tx(end)./tx) >= Dslope);
            numPoint = sum(msk);
            if numPoint <= DminNumPoint
                warning('%d T%s v%s', numPoint, env.Tstr{iT}, env.vstr{iv});
            end
            % TODO: optimum method to get D?
%             D(iT,iv) = mean(gx(msk) ./ tx(msk)) / 4;
%             p = polyfit(log(tx(msk)), log(gx(msk)), 1); D(iT,iv) = exp(p(2))/4;
            p = polyfit(tx(msk), g{iT,iv}(msk), 1); D(iT,iv) = p(1)/4;
            Dmask{iT,iv} = msk;
            dlmsave(fullfile(datavdir, 'D'), D(iT,iv));
        end
    end
    dlmsave(fullfile(env.datadir, 'D'), D);
    
    % Same T
    for iT = 1:env.nT
        if ~env.exhaustive; continue; end
        fignew('t', gLabel, 'xy');
        title(sprintf('T = %.3f', env.T(iT)));
        for iv = 1:env.nv
            if env.errbar
                h = errorbar(t{iT,iv}, g{iT,iv}, gerr{iT,iv});
            else
                h = plot(t{iT,iv}, g{iT,iv});
            end
            if env.plotfit
                tref = t{iT,iv}(Dmask{iT,iv});
                plot(tref, 4*D(iT,iv)*tref, '--', 'color', get(h, 'Color'));
            end
        end
        figsave(fullfile(msddir, ['msd_T' env.Tstr{iT}]));
    end
    % Same v
    for iv = 1:env.nv
        if ~env.exhaustive && iv ~= env.iv; continue; end
        fignew('t', gLabel, 'xy');
        xlabel('$t$', 'Interpreter', 'LaTex');
        ylabel('$\left\langle |\mathbf{r}_l(t) - \mathbf{r}_l(0)|^2 \right\rangle$', 'Interpreter', 'LaTex');
        if env.exhaustive
            title(sprintf('\\phi_v = %.3f', env.v(iv)));
        end
        for iT = 1:env.nT
            if env.errbar
                h = errorbar(t{iT,iv}, g{iT,iv}, gerr{iT,iv});
            else
                h = plot(t{iT,iv}, g{iT,iv});
            end
            if env.plotfit
                tref = t{iT,iv}(Dmask{iT,iv});
                plot(tref, 4*D(iT,iv)*tref, '--', 'color', get(h, 'Color'));
            end
        end
        if ~env.exhaustive
            xlim([1e-9 1e7]);
            subnumber '(a)';
        end
        figsave(fullfile(msddir, ['msd_v' env.vstr{iv}]));
    end
    
    % Facilitation
    if env.exhaustive && 0
        % Plot D vs v in linear scale seperately at each T
        for iT = 1:env.nT
            fignew('\phi_v', 'D');
            title(sprintf('T = %.3f', env.T(iT)));
            plot(env.v, D(iT,:), '+');
            xlim([0 max(xlim)]);
            ylim([0 max(ylim)]);
            figsave(fullfile(msddir, ['Dv_T' env.Tstr{iT}]));
        end
    end
    
    % alpha (D~v exponent)
    vmask = env.v <= FACvMax;
    for iT = 1:env.nT
        p = polyfit(log(env.v(vmask)), log(D(iT,vmask)), 1);
        a(iT) = p(1); 
        b(iT) = p(2);
    end

    % Plot D vs v at each T together
    fignew('\phi_v', 'D', 'xy');
    xlabel('$\phi_v$', 'Interpreter', 'LaTex');
    ylabel('$D$', 'Interpreter', 'LaTex');
    for iT = 1:env.nT
        h = plot(env.v, D(iT,:), '+', 'markersize', 3);
        plot(env.v, exp(a(iT)*log(env.v)+b(iT)), '-', ...
            'linewidth', 0.2, 'color', get(h, 'color'));
    end
    xlim([0.003 1]); ylim([1e-6 1e+6]);
    subnumber '(a)';
    figsave(fullfile(msddir, 'Dv'), 5, 3.5);
    
%     % Inset
%     pos = [0.665 0.225 0.216 0.225];
%     axes('Position', pos, 'FontSize', 8);
%     labels('1/T', '\alpha');
%     box on;
    fignew('1/T', '\alpha');
    xlabel('$1/T$', 'Interpreter', 'LaTex');
    plot(env.invT, a, '+-b')
    xlim([0 6]); ylim([1 3]);
    subnumber '(b)';
    figsave(fullfile(msddir, 'alphaT'));

    % Super-Arrhenius
    % D vs 1/T together at each v
    vstep = 2;
    disp(env.v(1:vstep:env.nv)');
    fignew('1/T', 'D', 'y');
    xlabel('$1/T$', 'Interpreter', 'LaTex');
    ylabel('$D$', 'Interpreter', 'LaTex');
    for iv = 1:vstep:env.nv
        plot(env.invT, D(:,iv), '-', 'linewidth', 0.2);
    end
    xlim([0 6]); ylim([1e-6 1e+6]);
    subnumber '(b)';
    figsave(fullfile(msddir, 'DT'));
    
    Dliqf = @(T,v) v/(1-v)*env.w0*exp(-env.EB0./T);
    fignew('1/T', 'D', 'y');
    for iv = 1:vstep:env.nv
        beta = [0 6];
        v = env.v(iv);
        plot(beta, Dliqf(1./beta, v), 'r', 'linewidth', 0.1);
        if iv == 1; legend('D_{liq}(T,v)'); end
        plot(env.invT, D(:,iv), '-', 'linewidth', 0.2);
    end
    xlim([0 6]); ylim([1e-6 1e+6]);
    text(0.2, 1e-1, ...
        '$w = w_0 \exp\left(-\frac{E_{B0}+\Delta E/2}{T}\right)$ where $\Delta E \in [-1.5, 1.5]$', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    text(0.2, 1e-2, 'Consider $T \to\infty$ and $\phi_v \to 0$. Assume that', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    text(0.2, 1e-3, 'each void does simple RW with step size $\varepsilon = 1$ and interval $\delta t = \frac{1}{4w}$', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    text(0.2, 1e-4, 'Void diffusion coefficient then given by $D_{liq}^{void} = \frac{\varepsilon^2}{4\delta t} = w$', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    text(0.2, 1e-5, 'Thus $D_{liq} = \frac{\phi_v}{1-\phi_v}D_{liq}^{void} = \frac{\phi_v}{1-\phi_v}w_0\exp(-E_{B0}/T)$, which seems overestimation twice as actual.', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    figsave(fullfile(msddir, 'DT2'));

    Dliqf = @(T,v) 1/2*v/(1-v)*env.w0*exp(-env.EB0./T);
    fignew('1/T', 'D', 'y');
    for iv = 1:vstep:env.nv
        beta = [0 6];
        v = env.v(iv);
        plot(beta, Dliqf(1./beta, v), 'r', 'linewidth', 0.1);
        if iv == 1; legend('0.5 D_{liq}(T,v)'); end
        plot(env.invT, D(:,iv), '-', 'linewidth', 0.2);
    end
    xlim([0 6]); ylim([1e-6 1e6]);
    text(0.2, 1e-5, 'With the $1/2$ correction factor the prediction looks perfect for small $\phi_v$ (lower curves).', ...
        'Interpreter', 'Latex', 'fontsize', 6);
    figsave(fullfile(msddir, 'DT3'));

    % 3D: D(T,v)
    [T,V] = meshgrid(env.v, 1./env.T);
    figure;
    surf(V, T, D);
    labels('1/T', '\phi_v', 'D');
    logscale yz;
    savefig(fullfile(msddir, 'DTv'));
    if strcmp(env.visible, 'off'); close; end
    
end
