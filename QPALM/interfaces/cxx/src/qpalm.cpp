#include <qpalm.hpp>

#include <qpalm.h> // qpalm_setup, qpalm_solve, etc.

namespace qpalm {

const ::QPALMData *Data::get_c_data_ptr() const {
    data.n = static_cast<size_t>(n);
    data.m = static_cast<size_t>(m);
    data.Q = Q.get();
    data.A = A.get();
    // Casting away const is fine, since we know that the QPALM C API doesn't
    // write to these vectors (it even creates a copy of them in the workspace).
    data.q    = const_cast<c_float *>(q.data());
    data.c    = c;
    data.bmin = const_cast<c_float *>(bmin.data());
    data.bmax = const_cast<c_float *>(bmax.data());
    return &data;
}

Settings::Settings() { ::qpalm_set_default_settings(this); }

using QPALMInfo = ::QPALMInfo;

Solver::Solver(const Data &data, const Settings &settings)
    : work{::qpalm_setup(data.get_c_data_ptr(), &settings)} {}

void Solver::update_settings(const Settings &settings) {
    ::qpalm_update_settings(work.get(), &settings);
}

void Solver::update_bounds(std::optional<const_ref_vec_t> bmin,
                           std::optional<const_ref_vec_t> bmax) {
    ::qpalm_update_bounds(work.get(), bmin ? bmin->data() : nullptr,
                          bmax ? bmax->data() : nullptr);
}

void Solver::update_q(const_ref_vec_t q) {
    ::qpalm_update_q(work.get(), q.data());
}

void Solver::update_Q_A(const_ref_vec_t Q_vals, const_ref_vec_t A_vals) {
    ::qpalm_update_Q_A(work.get(), Q_vals.data(), A_vals.data());
}

void Solver::warm_start(std::optional<const_ref_vec_t> x,
                        std::optional<const_ref_vec_t> y) {
    ::qpalm_warm_start(work.get(), x ? x->data() : nullptr,
                       y ? y->data() : nullptr);
}

void Solver::solve() { ::qpalm_solve(work.get()); }

SolutionView Solver::get_solution() const {
    assert(work->solution);
    assert(work->solution->x);
    assert(work->solution->y);
    auto en = static_cast<Eigen::Index>(work->data->n);
    auto em = static_cast<Eigen::Index>(work->data->m);
    return {
        {work->solution->x, en},
        {work->solution->y, em},
    };
}

const QPALMInfo &Solver::get_info() const {
    assert(work->info);
    return *work->info;
}

const_borrowed_vec_t Solver::get_prim_inf_certificate() const {
    auto em = static_cast<Eigen::Index>(work->data->m);
    return {work->delta_y, em};
}

const_borrowed_vec_t Solver::get_dual_inf_certificate() const {
    auto en = static_cast<Eigen::Index>(work->data->n);
    return {work->delta_x, en};
}

void alloc::qpalm_workspace_cleaner::operator()(::QPALMWorkspace *w) const {
    ::qpalm_cleanup(w);
}

} // namespace qpalm