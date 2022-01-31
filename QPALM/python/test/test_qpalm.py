import numpy as np
import scipy.sparse as sp
import qpalm as qp


def test_simple_3x4():

    data = qp.QPALMData(3, 4)

    row = np.array([0, 0, 1, 1])
    col = np.array([0, 1, 0, 1])
    val = np.array([1, -1, -1, 2])
    data.Q = sp.csc_matrix((val, (row, col)), shape=(3, 3))

    data.q = np.array([-2, -6, 1])
    data.c = 0
    data.bmin = np.array([0.5, -10, -10, -10])
    data.bmax = np.array([0.5, 10, 10, 10])

    row = np.array([0, 1, 0, 2, 0, 3])
    col = np.array([0, 0, 1, 1, 2, 2])
    val = np.array([1, 1, 1, 1, 1, 1])
    data.A = sp.csc_matrix((val, (row, col)), shape=(4, 3))

    settings = qp.QPALMSettings()
    solver = qp.QPALMSolver(data, settings)
    solver.solve()
    sol_x = solver.solution.x
    print(sol_x, solver.info.iter)
    tol = 1e-3
    assert abs(sol_x[0] - 5.5) < tol
    assert abs(sol_x[1] - 5.0) < tol
    assert abs(sol_x[2] - (-10)) < tol

    solver.warm_start(solver.solution.x, solver.solution.y)
    solver.solve()
    print(sol_x, solver.info.iter)


def test_solution_lifetime():

    def scope():
        data = qp.QPALMData(3, 4)
        row = np.array([0, 0, 1, 1])
        col = np.array([0, 1, 0, 1])
        val = np.array([1, -1, -1, 2])
        data.Q = sp.csc_matrix((val, (row, col)), shape=(3, 3))

        data.q = np.array([-2, -6, 1])
        data.c = 0
        data.bmin = np.array([0.5, -10, -10, -10])
        data.bmax = np.array([0.5, 10, 10, 10])

        row = np.array([0, 1, 0, 2, 0, 3])
        col = np.array([0, 0, 1, 1, 2, 2])
        val = np.array([1, 1, 1, 1, 1, 1])
        data.A = sp.csc_matrix((val, (row, col)), shape=(4, 3))

        settings = qp.QPALMSettings()
        solver = qp.QPALMSolver(data, settings)
        solver.solve()
        sol_x = solver.solution.x
        return sol_x

    sol_x = scope()
    print(sol_x)
    tol = 1e-3
    assert abs(sol_x[0] - 5.5) < tol
    assert abs(sol_x[1] - 5.0) < tol
    assert abs(sol_x[2] - (-10)) < tol
