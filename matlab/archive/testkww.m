close all;
A = 0.8;
b = 0.8;
noise = 1e-3;
tau = 1;
t = 10.^(linspace(-2,2));
phi = A * exp(-(t/tau).^beta);
phi = phi + randn(size(phi))*noise;


figure;
semilogx(t, phi);
hold on;
grid on;

mask = phi > 5*noise;
x = t(mask); y = -log(phi(mask));
figure;
loglog(x, y);
hold on;
grid on;

mask = mask & phi < 0.5;
tm = t(mask); phim = phi(mask);
x = t(mask); y = -log(phi(mask));
p = polyfit(log(x), log(y), 1);
best = p(1); mblt = p(2);

disp(best);


