#pragma once

#include <cxx/sparse.hpp>

#include <types.h> // ::QPALMData, ::QPALMSettings, ::QPALMSolution, ::QPALMInfo

namespace qpalm {

namespace alloc {
struct qpalm_workspace_cleaner {
    void operator()(::QPALMWorkspace *) const;
};
} // namespace alloc

class QPALMData {
  public:
    index_t n, m;
    ladel_sparse_matrix_ptr Q = eigen_to_ladel_copy({n, n});
    ladel_sparse_matrix_ptr A = eigen_to_ladel_copy({m, n});
    c_float c                 = 0;
    vec_t q                   = vec_t::Zero(n);
    vec_t bmin                = vec_t::Zero(m);
    vec_t bmax                = vec_t::Zero(m);

  public:
    QPALMData(index_t n, index_t m) : n{n}, m{m} {}

    void set_Q(const sparse_mat_t &Q) { this->Q = eigen_to_ladel_copy(Q); }
    void set_A(const sparse_mat_t &A) { this->A = eigen_to_ladel_copy(A); }
    const ::QPALMData &get_c_data() const;

  private:
    mutable ::QPALMData data{};
};

struct QPALMSettings : ::QPALMSettings {
    QPALMSettings();
};

using ::QPALMInfo;

/// @note   This is just a view of the solution, which is invalidated when the
///         solver object is destroyed. Create a copy of @c x and @c y as type
///         @c vec_t if you need the solution after the solver is gone.
struct QPALMSolutionView {
    using const_borrowed_vec_t = Eigen::Map<const vec_t>;
    const_borrowed_vec_t x{nullptr, 0};
    const_borrowed_vec_t y{nullptr, 0};
};

/// Main QPALM solver.
///
/// @see ::qpalm_solve
///
/// @todo   - warm_start(x, y)
///         - update_bounds
///         - update_q
///         - update_settings
class QPALMSolver {
  public:
    QPALMSolver(const QPALMData &data, const QPALMSettings &settings);

    void solve();

    /// @note   Returns a view which is only valid as long as the solver is not
    ///         destroyed.
    QPALMSolutionView get_solution() const;
    /// @note   Returns a reference that is only valid as long as the solver is
    ///         not destroyed.
    const QPALMInfo &get_info() const;


  private:
    using workspace_ptr =
        std::unique_ptr<::QPALMWorkspace, alloc::qpalm_workspace_cleaner>;
    workspace_ptr work;
};

} // namespace qpalm