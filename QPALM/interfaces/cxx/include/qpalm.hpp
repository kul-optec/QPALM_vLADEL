#pragma once

#include <sparse.hpp>

#include <types.h> // ::QPALMData, ::QPALMSettings, ::QPALMSolution, ::QPALMInfo

#include <optional>

/**
 * @defgroup qpalm-cxx-grp C++ Interface
 *
 * This is a C++ interface of the QPALM solver that provides a solver class to 
 * help with resource management, and with interoperability with Eigen matrices
 * and vectors.
 */

/// @see    @ref qpalm-cxx-grp
namespace qpalm {
/// RAII-based wrappers for the allocation and deallocation functions of the C
/// API.
namespace alloc {
/// Callable that cleans up the given workspace.
struct qpalm_workspace_cleaner {
    QPALM_CXX_EXPORT void operator()(::QPALMWorkspace *) const;
};
} // namespace alloc

/**
 * Stores the matrices and vectors that define the problem.
 * @f{align*}{ 
 * & \operatorname*{\mathrm{minimize}}_x
 *      & & \tfrac{1}{2}x^\top Q x + q^\top x + c \\
 * & \text{subject to}
 *      & & b_\mathrm{min} \leq Ax \leq b_\mathrm{max}
 * @f}
 * @ingroup qpalm-cxx-grp
 */
class Data {
  public:
    /// Problem dimension
    /// (size of x and q, number rows and columns of Q, number of columns of A).
    index_t n;
    /// Number of constraints
    /// (size of bmin and bmax, number of rows of A).
    index_t m;
    ladel_sparse_matrix_ptr Q = eigen_to_ladel_copy({n, n});
    ladel_sparse_matrix_ptr A = eigen_to_ladel_copy({m, n});
    c_float c                 = 0;
    vec_t q                   = vec_t::Zero(n);
    vec_t bmin                = vec_t::Zero(m);
    vec_t bmax                = vec_t::Zero(m);

  public:
    /// Construct a problem of dimension n with m constraints.
    Data(index_t n, index_t m) : n{n}, m{m} {}

    /// Set the sparse Q matrix. Creates a copy.
    void set_Q(const sparse_mat_t &Q) {
        assert(Q.rows() == n);
        assert(Q.cols() == n);
        this->Q = eigen_to_ladel_copy(Q);
    }
    /// Set the sparse A matrix. Creates a copy.
    void set_A(const sparse_mat_t &A) {
        assert(A.rows() == m);
        assert(A.cols() == n);
        this->A = eigen_to_ladel_copy(A);
    }
    /// Get a pointer to the underlying C data structure.
    /// @see    @ref ::QPALMData
    QPALM_CXX_EXPORT const ::QPALMData *get_c_data_ptr() const;

    /// Get a view on the Q matrix of the problem.
    sparse_mat_view_t get_Q() const {
        return {static_cast<index_t>(Q->nrow),
                static_cast<index_t>(Q->ncol),
                static_cast<index_t>(Q->nzmax),
                Q->p,
                Q->i,
                Q->x,
                Q->nz};
    }
    /// Get a view on the A matrix of the problem.
    sparse_mat_view_t get_A() const {
        return {static_cast<index_t>(A->nrow),
                static_cast<index_t>(A->ncol),
                static_cast<index_t>(A->nzmax),
                A->p,
                A->i,
                A->x,
                A->nz};
    }

  private:
    // Underlying C data structure that is passed to the solver.
    mutable ::QPALMData data{};
};

/**
 * Settings and parameters for the QPALM solver.
 * @ingroup qpalm-cxx-grp
 */
struct Settings : ::QPALMSettings {
    /// Construct with default settings.
    /// @see    @ref qpalm_set_default_settings
    QPALM_CXX_EXPORT Settings();
};

/**
 * Information returned by the solver.
 * @ingroup qpalm-cxx-grp
 */

using Info = ::QPALMInfo;

/**
 * View on the solution returned by the solver.
 * @note   This is just a view of the solution, which is invalidated when the
 *         solver object is destroyed. Create a copy of @c x and @c y as type
 *         @c vec_t if you need the solution after the solver is gone.
 */
struct SolutionView {
    const_borrowed_vec_t x{nullptr, 0};
    const_borrowed_vec_t y{nullptr, 0};
};

/**
 * Main QPALM solver.
 *
 * @see    @ref ::qpalm_solve
 * @ingroup qpalm-cxx-grp
 */
class Solver {
  public:
    /// Create a new solver for the problem defined by @p data and with the
    /// parameters defined by @p settings.
    QPALM_CXX_EXPORT Solver(const Data &data, const Settings &settings);

    /// @see    @ref ::qpalm_update_settings
    QPALM_CXX_EXPORT void update_settings(const Settings &settings);
    /// @see    @ref ::qpalm_update_bounds
    QPALM_CXX_EXPORT void update_bounds(std::optional<const_ref_vec_t> bmin,
                                        std::optional<const_ref_vec_t> bmax);
    /// @see    @ref ::qpalm_update_q
    QPALM_CXX_EXPORT void update_q(const_ref_vec_t q);
    /// @see    @ref ::qpalm_update_Q_A
    /// @note   Updates only the values, sparsity pattern should remain the
    ///         same.
    QPALM_CXX_EXPORT void update_Q_A(const_ref_vec_t Q_vals,
                                     const_ref_vec_t A_vals);

    /// @see    @ref ::qpalm_warm_start
    QPALM_CXX_EXPORT void warm_start(std::optional<const_ref_vec_t> x,
                                     std::optional<const_ref_vec_t> y);

    /// Solve the problem. The solution will be available through
    /// @ref get_solution() and the solver information and statistics through
    /// @ref get_info().
    /// @see    @ref ::qpalm_solve
    QPALM_CXX_EXPORT void solve();

    /// Get the solution computed by @ref solve().
    /// @note   Returns a view that is only valid as long as the solver is not
    ///         destroyed.
    /// @see    @ref QPALMWorkspace::solution
    QPALM_CXX_EXPORT SolutionView get_solution() const;
    /// Get the solver information from the last call to @ref solve().
    /// @note   Returns a reference that is only valid as long as the solver is
    ///         not destroyed.
    QPALM_CXX_EXPORT const QPALMInfo &get_info() const;

    /// Get the certificate of primal infeasibility of the problem.
    QPALM_CXX_EXPORT const_borrowed_vec_t get_prim_inf_certificate() const;
    /// Get the certificate of dual infeasibility of the problem.
    QPALM_CXX_EXPORT const_borrowed_vec_t get_dual_inf_certificate() const;

    /// Get the problem dimension @f$ n @f$ (size of @f$ x @f$).
    /// @see    @ref QPALMData::n
    index_t get_n() const { return work->data->n; }
    /// Get the number of constraints @f$ m @f$.
    /// @see    @ref QPALMData::m
    index_t get_m() const { return work->data->m; }

    /// Get a pointer to the underlying C workspace data structure.
    /// @see    @ref ::QPALMWorkspace
    QPALM_CXX_EXPORT const ::QPALMWorkspace *get_c_work_ptr() const {
        return work.get();
    }

  private:
    using workspace_ptr =
        std::unique_ptr<::QPALMWorkspace, alloc::qpalm_workspace_cleaner>;
    workspace_ptr work;
};

} // namespace qpalm