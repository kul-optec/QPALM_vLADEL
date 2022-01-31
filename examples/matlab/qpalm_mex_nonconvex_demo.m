%A demo to show how the matlab interface for qpalm is called for a
%nonconvex QP

import qpalm.qpalm

%% Generate data
Q = sparse([2 0; 0 -3]);
q = [1;2];
m = 2;
A = speye(2);
lb = -2*ones(m,1);
ub = 2*ones(m,1);

%% Solve with qpalm
solver = qpalm;
settings = solver.default_settings();
%IMPORTANT: set nonconvex to true for nonconvex QPs
settings.nonconvex = true;

solver.setup(Q, q, A, lb, ub, settings); 
res = solver.solve();

fprintf('Elapsed time: %f seconds\n', res.info.run_time);
fprintf('Status: %s\n', res.info.status);
res.x
