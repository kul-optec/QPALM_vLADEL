from copy import deepcopy
import qpalm
import numpy as np
import concurrent.futures
import pytest
import os
import scipy.sparse as spa


def test_qpalm_threaded():
    valgrind = "valgrind" in os.getenv("LD_PRELOAD", "")

    settings = qpalm.Settings()
    settings.max_iter = 300
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
        data.Q = spa.csc_array(Q)
        data.A = spa.csc_array(A)
        data.q = rng.random(n)
        data.bmax = rng.random(m)
        data.bmin = -np.inf * np.ones(m)
        return qpalm.Solver(data=data, settings=settings)

    shared_solver = create_solver()

    def good_experiment():
        solver = create_solver()
        solver.solve(asynchronous=True)
        return solver.info.status_val == qpalm.Info.MAX_ITER_REACHED

    def bad_experiment():
        solver = shared_solver
        solver.solve(asynchronous=True)
        return solver.info.status_val == qpalm.Info.MAX_ITER_REACHED

    def run(experiment):
        N = 4 if valgrind else 200
        with concurrent.futures.ThreadPoolExecutor(max_workers=os.cpu_count()) as pool:
            futures = (pool.submit(experiment) for _ in range(N))
            for future in concurrent.futures.as_completed(futures):
                success = future.result()
                assert success

    run(good_experiment)
    if not valgrind:
        with pytest.raises(
            RuntimeError, match=r"^Same instance of .* used in multiple threads"
        ) as e:
            run(bad_experiment)
        print(e.value)


if __name__ == "__main__":
    test_qpalm_threaded()
    print("done.")
