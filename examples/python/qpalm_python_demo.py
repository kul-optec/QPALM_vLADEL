import scipy.sparse as sp
import qpalm

# %% Construct the matrices describing the problem
"""
      minimize    ½〈x, Qx〉+〈q, x〉+ c
         x
      subject to  bmin ≤ Ax ≤ bmax
"""
data = qpalm.Data(3, 4)

# Q is sparse and symmetric
row = [0, 0, 1, 1, 2]
col = [0, 1, 0, 1, 2]
valuesQ = [1, -1, -1, 2, 1]
data.Q = sp.csc_matrix((valuesQ, (row, col)), shape=(3, 3))

# q, bmin and bmax are dense vectors
data.q = [-2, -6, 1]
data.bmin = [0.5, -10, -10, -10]
data.bmax = [0.5, 10, 10, 10]

# A is sparse and rectangular
row = [0, 1, 0, 2, 0, 3]
col = [0, 0, 1, 1, 2, 2]
valuesA = [1, 1, 1, 1, 1, 1]
data.A = sp.csc_matrix((valuesA, (row, col)), shape=(4, 3))

# %% Configure the solver

settings = qpalm.Settings()
settings.eps_abs = 1e-8

# %% Solve the problem

solver = qpalm.Solver(data, settings)
solver.solve()

# %% Print the results
print("Status:     ", solver.info.status)
print("Solution:   ", solver.solution.x)
print("Multipliers:", solver.solution.y)
assert solver.info.status_val == qpalm.Info.SOLVED

# %% Warm start with solution 
solver.warm_start(solver.solution.x, solver.solution.y)
solver.solve()
print(solver.solution.x)

# %% Update functions

# It is possible to update the settings, the bounds, the linear part of the 
# cost (q) and the values of matrices Q and A

settings.eps_abs = 1e-10
settings.eps_rel = 0
solver.update_settings(settings)
solver.solve()
print(solver.solution.x)

data.bmin = [0, 0, -15, 1]
solver.update_bounds(bmin=data.bmin)
solver.solve()
print(solver.solution.x)

data.q = [1, 0, -2]
solver.update_q(data.q)
solver.solve()
print(solver.solution.x)

# Note that QPALM internally only uses the upper-triangular part of Q, so when
# updating the values of Q, you have to pass the data of the upper-triangular
# part only. 
# If you need to update the sparsity pattern, create a new solver.
Qup = sp.triu(data.Q)
Qup.data[1] = -0.5
solver.update_Q_A(Qup.data, valuesA)
solver.solve()
print(solver.solution.x)
