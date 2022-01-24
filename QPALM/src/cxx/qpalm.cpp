#include <cxx/qpalm.hpp>

#include <qpalm.h> // qpalm_setup, qpalm_solve, etc.

namespace qpalm {

const ::QPALMData &QPALMData::get_c_data() const {
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
    return data;
}

QPALMSettings::QPALMSettings() { ::qpalm_set_default_settings(this); }

using QPALMInfo = ::QPALMInfo;

QPALMSolver::QPALMSolver(const QPALMData &data, const QPALMSettings &settings)
    : work{::qpalm_setup(&data.get_c_data(), &settings)} {}

void QPALMSolver::solve() { ::qpalm_solve(work.get()); }

QPALMSolutionView QPALMSolver::get_solution() const {
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

const QPALMInfo &QPALMSolver::get_info() const {
    assert(work->info);
    return *work->info;
}
void alloc::qpalm_workspace_cleaner::operator()(::QPALMWorkspace *w) const {
    ::qpalm_cleanup(w);
}

} // namespace qpalm