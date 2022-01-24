/**
 * @file    C++ extension for Python interface of QPALM.
 */

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;
using py::operator""_a;

#include <cxx/qpalm.hpp>
#include <cxx/sparse.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

void check_dim(const qpalm::sparse_mat_t &M, std::string_view name, qpalm::index_t r,
               qpalm::index_t c) {
    if (M.rows() != r)
        throw std::invalid_argument("Invalid number of rows for '" + std::string(name) + "' (got " +
                                    std::to_string(M.rows()) + ", should be " + std::to_string(r) +
                                    ")");
    if (M.cols() != c)
        throw std::invalid_argument("Invalid number of columns for '" + std::string(name) +
                                    "' (got " + std::to_string(M.cols()) + ", should be " +
                                    std::to_string(c) + ")");
}

void check_dim(const qpalm::vec_t &v, std::string_view name, qpalm::index_t r) {
    if (v.rows() != r)
        throw std::invalid_argument("Invalid number of rows for '" + std::string(name) + "' (got " +
                                    std::to_string(v.rows()) + ", should be " + std::to_string(r) +
                                    ")");
}

PYBIND11_MODULE(MODULE_NAME, m) {
    m.doc()               = "C++ implementation of QPALM";
    m.attr("__version__") = VERSION_INFO;

    py::class_<qpalm::QPALMData>(m, "QPALMData")
        .def(py::init<qpalm::index_t, qpalm::index_t>(), "n"_a, "m"_a)
        .def_property(
            "Q",
            [](qpalm::QPALMData &) -> qpalm::sparse_mat_t {
                throw std::logic_error("Not implemented");
            },
            [](qpalm::QPALMData &d, qpalm::sparse_mat_t Q) {
                check_dim(Q, "Q", d.n, d.n);
                d.set_Q(std::move(Q));
            })
        .def_property(
            "A",
            [](qpalm::QPALMData &) -> qpalm::sparse_mat_t {
                throw std::logic_error("Not implemented");
            },
            [](qpalm::QPALMData &d, qpalm::sparse_mat_t A) {
                check_dim(A, "A", d.m, d.n);
                d.set_A(std::move(A));
            })
        .def_property(
            "q",
            [](qpalm::QPALMData &) -> qpalm::vec_t { throw std::logic_error("Not implemented"); },
            [](qpalm::QPALMData &d, qpalm::vec_t q) {
                check_dim(q, "q", d.n);
                d.q = (std::move(q));
            })
        .def_readwrite("c", &qpalm::QPALMData::c)
        .def_property(
            "bmin",
            [](qpalm::QPALMData &) -> qpalm::vec_t { throw std::logic_error("Not implemented"); },
            [](qpalm::QPALMData &d, qpalm::vec_t b) {
                check_dim(b, "bmin", d.m);
                d.bmin = std::move(b);
            })
        .def_property(
            "bmax",
            [](qpalm::QPALMData &) -> qpalm::vec_t { throw std::logic_error("Not implemented"); },
            [](qpalm::QPALMData &d, qpalm::vec_t b) {
                check_dim(b, "bmax", d.m);
                d.bmax = std::move(b);
            });

    py::class_<qpalm::QPALMSolutionView>(m, "QPALMSolution")
        .def_readonly("x", &qpalm::QPALMSolutionView::x)
        .def_readonly("y", &qpalm::QPALMSolutionView::y);

    py::class_<qpalm::QPALMInfo>(m, "QPALMInfo")
        .def_readwrite("iter", &qpalm::QPALMInfo::iter)
        .def_readwrite("iter_out", &qpalm::QPALMInfo::iter_out)
        // .def_readwrite("status", &qpalm::QPALMInfo::status)
        .def_readwrite("status_val", &qpalm::QPALMInfo::status_val)
        .def_readwrite("pri_res_norm", &qpalm::QPALMInfo::pri_res_norm)
        .def_readwrite("dua_res_norm", &qpalm::QPALMInfo::dua_res_norm)
        .def_readwrite("dua2_res_norm", &qpalm::QPALMInfo::dua2_res_norm)
        .def_readwrite("objective", &qpalm::QPALMInfo::objective)
        .def_readwrite("dual_objective", &qpalm::QPALMInfo::dual_objective)
#ifdef PROFILING
        .def_readwrite("setup_time", &qpalm::QPALMInfo::setup_time)
        .def_readwrite("solve_time", &qpalm::QPALMInfo::solve_time)
        .def_readwrite("run_time", &qpalm::QPALMInfo::run_time)
#endif
        .def_property(
            "status", [](const qpalm::QPALMInfo &i) -> std::string_view { return i.status; },
            [](qpalm::QPALMInfo &i, std::string_view s) {
                constexpr auto maxsize = sizeof(i.status);
                if (s.size() >= maxsize)
                    throw std::out_of_range("Status string too long (maximum is " +
                                            std::to_string(maxsize - 1) + ")");
                std::copy_n(s.data(), s.size(), i.status);
                i.status[s.size()] = '\0';
            });

    py::class_<qpalm::QPALMSettings>(m, "QPALMSettings")
        .def(py::init())
        .def_readwrite("max_iter", &qpalm::QPALMSettings::max_iter)
        .def_readwrite("inner_max_iter", &qpalm::QPALMSettings::inner_max_iter)
        .def_readwrite("eps_abs", &qpalm::QPALMSettings::eps_abs)
        .def_readwrite("eps_rel", &qpalm::QPALMSettings::eps_rel)
        .def_readwrite("eps_abs_in", &qpalm::QPALMSettings::eps_abs_in)
        .def_readwrite("eps_rel_in", &qpalm::QPALMSettings::eps_rel_in)
        .def_readwrite("rho", &qpalm::QPALMSettings::rho)
        .def_readwrite("eps_prim_inf", &qpalm::QPALMSettings::eps_prim_inf)
        .def_readwrite("eps_dual_inf", &qpalm::QPALMSettings::eps_dual_inf)
        .def_readwrite("theta", &qpalm::QPALMSettings::theta)
        .def_readwrite("delta", &qpalm::QPALMSettings::delta)
        .def_readwrite("sigma_max", &qpalm::QPALMSettings::sigma_max)
        .def_readwrite("sigma_init", &qpalm::QPALMSettings::sigma_init)
        .def_readwrite("proximal", &qpalm::QPALMSettings::proximal)
        .def_readwrite("gamma_init", &qpalm::QPALMSettings::gamma_init)
        .def_readwrite("gamma_upd", &qpalm::QPALMSettings::gamma_upd)
        .def_readwrite("gamma_max", &qpalm::QPALMSettings::gamma_max)
        .def_readwrite("scaling", &qpalm::QPALMSettings::scaling)
        .def_readwrite("nonconvex", &qpalm::QPALMSettings::nonconvex)
        .def_readwrite("verbose", &qpalm::QPALMSettings::verbose)
        .def_readwrite("print_iter", &qpalm::QPALMSettings::print_iter)
        .def_readwrite("warm_start", &qpalm::QPALMSettings::warm_start)
        .def_readwrite("reset_newton_iter", &qpalm::QPALMSettings::reset_newton_iter)
        .def_readwrite("enable_dual_termination", &qpalm::QPALMSettings::enable_dual_termination)
        .def_readwrite("dual_objective_limit", &qpalm::QPALMSettings::dual_objective_limit)
        .def_readwrite("time_limit", &qpalm::QPALMSettings::time_limit)
        .def_readwrite("ordering", &qpalm::QPALMSettings::ordering)
        .def_readwrite("factorization_method", &qpalm::QPALMSettings::factorization_method)
        .def_readwrite("max_rank_update", &qpalm::QPALMSettings::max_rank_update)
        .def_readwrite("max_rank_update_fraction", &qpalm::QPALMSettings::max_rank_update_fraction);

    py::class_<qpalm::QPALMSolver>(m, "QPALMSolver")
        .def(py::init<const qpalm::QPALMData &, const qpalm::QPALMSettings &>(), "data"_a,
             "settings"_a)
        .def("solve", &qpalm::QPALMSolver::solve)
        .def_property_readonly("solution", &qpalm::QPALMSolver::get_solution)
        .def_property_readonly("info", &qpalm::QPALMSolver::get_info);
}