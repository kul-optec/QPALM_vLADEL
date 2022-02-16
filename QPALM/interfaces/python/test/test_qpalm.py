import numpy as np
import scipy.sparse as sp
import qpalm


def test_simple_3x4():

    data = qpalm.Data(3, 4)

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

    settings = qpalm.Settings()
    solver = qpalm.Solver(data, settings)
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
        data = qpalm.Data(3, 4)
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

        settings = qpalm.Settings()
        solver = qpalm.Solver(data, settings)
        solver.solve()
        sol_x = solver.solution.x
        return sol_x

    sol_x = scope()
    print(sol_x)
    tol = 1e-3
    assert abs(sol_x[0] - 5.5) < tol
    assert abs(sol_x[1] - 5.0) < tol
    assert abs(sol_x[2] - (-10)) < tol

def test_data_element_access():
    data = qpalm.Data(3, 4)
    row = np.array([0, 0, 1, 1])
    col = np.array([0, 1, 0, 1])
    val = np.array([1, -1, -1, 2])
    data.Q = sp.csc_matrix((val, (row, col)), shape=(3, 3))

    data.q = np.array([-2, -6, 1])
    data.c = 0
    data.bmin = np.array([0.5, -10, -11, -12])
    data.bmax = np.array([1.5, 10, 11, 12])

    row = np.array([0, 1, 0, 2, 0, 3])
    col = np.array([0, 0, 1, 1, 2, 2])
    val = np.array([1, 1, 1, 1, 1, 1])
    data.A = sp.csc_matrix((val, (row, col)), shape=(4, 3))

    assert data.q.flags.owndata == False
    assert data.q.flags.writeable == True

    assert np.all(data.q == np.array([-2, -6, 1]))
    data.q[2] = 7
    assert data.q[2] == 7
    assert np.all(data.q == np.array([-2, -6, 7]))

    assert data.bmin.flags.owndata == False
    assert data.bmin.flags.writeable == True

    assert np.all(data.bmin == np.array([0.5, -10, -11, -12]))
    data.bmin[3] = -99
    assert data.bmin[3] == -99
    assert np.all(data.bmin == np.array([0.5, -10, -11, -99]))

    assert data.bmax.flags.owndata == False
    assert data.bmax.flags.writeable == True

    assert np.all(data.bmax == np.array([1.5, 10, 11, 12]))
    data.bmax[1] = 99
    assert data.bmax[1] == 99
    assert np.all(data.bmax == np.array([1.5, 99, 11, 12]))

def test_data_lifetime():

    def get_q():
        data = qpalm.Data(3, 4)
        data.q = np.array([-2, -6, 1])
        return data.q

    q = get_q()
    assert np.all(q == np.array([-2, -6, 1]))
    q[1] = -7
    assert q[1] == -7
    assert np.all(q == np.array([-2, -7, 1]))
