import qpalm as qp
import numpy as np
import scipy.sparse as sp

data = qp.QPALMData(3, 4)

row = np.array([0, 0, 1, 1])
col = np.array([0, 1, 0, 1])
values = np.array([1, -1, -1, 2])
data.Q = sp.csc_matrix((values, (row, col)), shape=(3, 3))

data.q = np.array([-2, -6, 1])
data.bmin = np.array([0.5, -10, -10, -10])
data.bmax = np.array([0.5, 10, 10, 10])

row = np.array([0, 1, 0, 2, 0, 3])
col = np.array([0, 0, 1, 1, 2, 2])
values = np.array([1, 1, 1, 1, 1, 1])
data.A = sp.csc_matrix((values, (row, col)), shape=(4, 3))

settings = qp.QPALMSettings()
solver = qp.QPALMSolver(data, settings)
solver.solve()
sol_x = solver.solution.x
tol = 1e-5
assert abs(sol_x[0] - 5.5) < tol
assert abs(sol_x[1] - 5.0) < tol
assert abs(sol_x[2] - (-10)) < tol

# Warm start with solution to check whether the solver exits immediately
solver.warm_start(solver.solution.x, solver.solution.y)
solver.solve()
assert solver.info.iter == 0

# Update functions
# It is possible to update the bounds, the linear part of the cost (q) and the settings
settings.eps_abs = 1e-10
settings.eps_rel = 0

solver.update_settings(settings)
solver.solve()
sol_x = solver.solution.x
strict_tol = 1e-10
assert abs(sol_x[0] - 5.5) < strict_tol
assert abs(sol_x[1] - 5.0) < strict_tol
assert abs(sol_x[2] - (-10)) < strict_tol

settings.eps_abs = 1e-4
settings.eps_rel = 1e-4
solver.update_settings(settings)

data.bmin[3] = -15
solver.update_bounds(bmin=data.bmin)
solver.solve()
sol_x = solver.solution.x
assert abs(sol_x[0] - 8.5) < tol
assert abs(sol_x[1] - 7) < tol
assert abs(sol_x[2] - (-15)) < tol

sol_x[0] = 0
sol_x[1] = 0
sol_x[2] = 0

data.q[0] = 0
data.q[1] = 0
data.q[2] = 0
solver.update_q(data.q)
solver.solve()
sol_x = solver.solution.x
assert abs(sol_x[0] - 0) < tol
assert abs(sol_x[1] - 0) < tol
assert abs(sol_x[2] - 0.5) < tol
