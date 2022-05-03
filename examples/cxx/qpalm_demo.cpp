#include <qpalm.hpp>

#include <iostream>
#include <vector>

// Helper function to easily construct sparse matrices.
static auto make_sparse_matrix(qpalm::sp_index_t rows, qpalm::sp_index_t cols,
                               const std::vector<qpalm::triplet_t> &triplets) {
    qpalm::sparse_mat_t M{rows, cols};
    M.setFromTriplets(begin(triplets), end(triplets));
    return M;
}

int main() {

    // Define the problem
    // ------------------
    qpalm::sp_index_t n = 3; // Number of unknowns
    qpalm::sp_index_t m = 4; // Number of constraints
    qpalm::Data data    = {n, m};
    data.set_Q(make_sparse_matrix(n, n,
                                  {
                                      {0, 0, 1.0}, // row, col, value
                                      {1, 0, -1.0},
                                      {0, 1, -1.0},
                                      {1, 1, 2.0},
                                  }));
    data.q << -2, -6, 1;
    data.c = 0;
    data.set_A(make_sparse_matrix(m, n,
                                  {
                                      {0, 0, 1.0}, // row, col, value
                                      {1, 0, 1.0},
                                      {0, 1, 1.0},
                                      {2, 1, 1.0},
                                      {0, 2, 1.0},
                                      {3, 2, 1.0},
                                  }));
    data.bmin = qpalm::vec_t::Constant(m, -5.0);
    data.bmax = qpalm::vec_t::Constant(m, +8.0);

    // Configure the solver
    // --------------------
    qpalm::Settings settings;
    settings.eps_abs     = 1e-10;
    settings.eps_rel     = 1e-10;
    settings.max_iter    = 100;
    qpalm::Solver solver = {data, settings};

    // Solve the roblem
    // ----------------
    solver.solve();
    auto sol  = solver.get_solution();
    auto info = solver.get_info();

    // Print the results
    // -----------------
    std::cout << "Solver status: " << info.status << "\n"
              << "Iter:          " << info.iter << "\n"
              << "Iter Out:      " << info.iter_out << "\n";

#ifdef QPALM_TIMING
    std::cout << "Setup time:    " << info.setup_time << "\n"
              << "Solve time:    " << info.solve_time << "\n"
              << "Run time:      " << info.run_time << "\n\n";
#endif

    std::cout << "Solution:   \n"
              << sol.x.transpose() << "\n"
              << "Multipliers:\n"
              << sol.y.transpose() << "\n";
}