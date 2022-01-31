%A demo to show how the matlab interface for qpalm is called 

import qpalm.qpalm

%% Generate data
m = 300;n = 50;
A = sprandn(m, n, 1e-1, 1e-4); 

lb = -2*ones(m,1);
ub =  2*ones(m,1);
Q = sprandsym(n, 1e-1, 1e-4, 1);
q = 10*randn(n,1);

%% Solve with qpalm
solver = qpalm;
settings = solver.default_settings();
settings.delta = 100;
settings.proximal = true;
settings.scaling = 2;
settings.max_iter = 1000;
settings.eps_abs = 1e-3;
settings.eps_rel = 1e-3;

solver.setup(Q, q, A, lb, ub, settings); 
res = solver.solve();

fprintf('Elapsed time: %f seconds\n', res.info.run_time);
fprintf('Status: %s\n', res.info.status);

solver.delete();
