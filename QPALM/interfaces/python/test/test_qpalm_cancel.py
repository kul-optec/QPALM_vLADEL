import qpalm
import numpy as np
import concurrent.futures
from time import sleep
import scipy.sparse as spa


def test_qpalm_cancel():
    settings = qpalm.Settings()
    settings.max_iter = 20000
    settings.eps_abs = 1e-200
    settings.eps_rel = 0
    settings.eps_rel_in = 0
    settings.verbose = 1

    def create_solver():
        m, n = 100, 120
        data = qpalm.Data(n, m)
        rng = np.random.default_rng(seed=123)
        Q = rng.random((n, n))
        A = rng.random((m, n))
        Q = Q.T @ Q
        data.Q = spa.csc_matrix(Q)
        data.A = spa.csc_matrix(A)
        data.q = rng.random(n)
        data.bmax = rng.random(m)
        data.bmin = -np.inf * np.ones(m)
        return qpalm.Solver(data=data, settings=settings)

    solver = create_solver()

    def run_solver():
        solver.solve(asynchronous=True, suppress_interrupt=True)
        return solver.info.status_val

    with concurrent.futures.ThreadPoolExecutor() as pool:
        future = pool.submit(run_solver)
        sleep(0.2)
        solver.cancel()
        assert future.result() == qpalm.Info.USER_CANCELLATION


if __name__ == "__main__":
    test_qpalm_cancel()
    print("done.")
