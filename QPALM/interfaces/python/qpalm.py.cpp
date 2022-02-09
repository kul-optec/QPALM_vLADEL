/**
 * @file    C++ extension for Python interface of QPALM.
 */

#include <Python.h>
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using py::operator""_a;

#include <qpalm.hpp>
#include <sparse.hpp>

#include <algorithm>
#include <cstdarg>
#include <stdexcept>
#include <string>
#include <string_view>

/// Throw an exception if the dimensions of the matrix don't match the expected
/// dimensions @p r and @p c.
static void check_dim(const qpalm::sparse_mat_t &M, std::string_view name, qpalm::index_t r,
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

/// Throw an exception if the size of the vector doesn't match the expected
/// size @p r.
static void check_dim(const qpalm::vec_t &v, std::string_view name, qpalm::index_t r) {
    if (v.rows() != r)
        throw std::invalid_argument("Invalid number of rows for '" + std::string(name) + "' (got " +
                                    std::to_string(v.rows()) + ", should be " + std::to_string(r) +
                                    ")");
}

/// `printf`-style wrapper that prints to Python
static int print_wrap(const char *fmt, ...) LADEL_ATTR_PRINTF_LIKE;

#define PYBIND11_SUPPORTS_MAP_SPARSE_MATRIX 0

PYBIND11_MODULE(MODULE_NAME, m) {
    m.doc()               = "C and C++ implementation of QPALM";
    m.attr("__version__") = VERSION_INFO;

    ladel_set_alloc_config_calloc(&PyMem_Calloc);
    ladel_set_alloc_config_malloc(&PyMem_Malloc);
    ladel_set_alloc_config_realloc(&PyMem_Realloc);
    ladel_set_alloc_config_free(&PyMem_Free);
    ladel_set_print_config_printf(&print_wrap);

    py::class_<qpalm::QPALMData>(m, "QPALMData")
        .def(py::init<qpalm::index_t, qpalm::index_t>(), "n"_a, "m"_a)
        .def_property("Q",
#if PYBIND11_SUPPORTS_MAP_SPARSE_MATRIX
                      py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                          &qpalm::QPALMData::get_Q, py::return_value_policy::reference,
                          py::keep_alive<0, 1>()),
#else
                          [](const qpalm::QPALMData &d) -> qpalm::sparse_mat_t{
                              return d.get_Q();
                          },
#endif
                      [](qpalm::QPALMData &d, qpalm::sparse_mat_t Q) {
                          check_dim(Q, "Q", d.n, d.n);
                          d.set_Q(std::move(Q));
                      })
        .def_property("A",
#if PYBIND11_SUPPORTS_MAP_SPARSE_MATRIX
                      py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                          &qpalm::QPALMData::get_A, py::return_value_policy::reference,
                          py::keep_alive<0, 1>()),
#else
                          [](const qpalm::QPALMData &d) -> qpalm::sparse_mat_t{
                              return d.get_A();
                          },
#endif
                      [](qpalm::QPALMData &d, qpalm::sparse_mat_t A) {
                          check_dim(A, "A", d.m, d.n);
                          d.set_A(std::move(A));
                      })
        .def_property(
            "q", [](const qpalm::QPALMData &d) -> qpalm::vec_t { return d.q; },
            [](qpalm::QPALMData &d, qpalm::vec_t q) {
                check_dim(q, "q", d.n);
                d.q = (std::move(q));
            })
        .def_readwrite("c", &qpalm::QPALMData::c)
        .def_property(
            "bmin", [](const qpalm::QPALMData &d) -> qpalm::vec_t { return d.bmin; },
            [](qpalm::QPALMData &d, qpalm::vec_t b) {
                check_dim(b, "bmin", d.m);
                d.bmin = std::move(b);
            })
        .def_property(
            "bmax", [](const qpalm::QPALMData &d) -> qpalm::vec_t { return d.bmax; },
            [](qpalm::QPALMData &d, qpalm::vec_t b) {
                check_dim(b, "bmax", d.m);
                d.bmax = std::move(b);
            })
        .def("_get_c_data_ptr", &qpalm::QPALMData::get_c_data_ptr,
             "Return a pointer to the C data struct (of type ::QPALMData).",
             py::return_value_policy::reference_internal);

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
        .def(
            "update_settings",
            [](qpalm::QPALMSolver &self, const qpalm::QPALMSettings &settings) {
                self.update_settings(settings);
            },
            "settings"_a)
        .def(
            "update_bounds",
            [](qpalm::QPALMSolver &self, std::optional<qpalm::const_ref_vec_t> bmin,
               std::optional<qpalm::vec_t> bmax) {
                if (bmin)
                    check_dim(*bmin, "bmin", self.get_m());
                if (bmax)
                    check_dim(*bmax, "bmax", self.get_m());
                self.update_bounds(bmin, bmax);
            },
            "bmin"_a = py::none(), "bmax"_a = py::none())
        .def(
            "update_q",
            [](qpalm::QPALMSolver &self, qpalm::const_ref_vec_t q) {
                check_dim(q, "q", self.get_n());
                self.update_q(q);
            },
            "q"_a)
        .def(
            "update_Q_A",
            [](qpalm::QPALMSolver &self, qpalm::const_ref_vec_t Q_vals,
               qpalm::const_ref_vec_t A_vals) {
                check_dim(Q_vals, "Q_vals", self.get_c_work_ptr()->data->Q->nzmax);
                check_dim(A_vals, "A_vals", self.get_c_work_ptr()->data->A->nzmax);
                self.update_Q_A(Q_vals, A_vals);
            },
            "Q_vals"_a, "A_vals"_a)
        .def(
            "warm_start",
            [](qpalm::QPALMSolver &self, std::optional<qpalm::const_ref_vec_t> x,
               std::optional<qpalm::const_ref_vec_t> y) {
                if (x)
                    check_dim(*x, "x", self.get_n());
                if (y)
                    check_dim(*y, "y", self.get_m());
                self.warm_start(x, y);
            },
            "x"_a = py::none(), "y"_a = py::none())
        .def("solve", &qpalm::QPALMSolver::solve)
        .def_property_readonly("solution",
                               py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                                   &qpalm::QPALMSolver::get_solution,
                                   py::return_value_policy::reference, py::keep_alive<0, 1>()))
        .def_property_readonly("info",
                               py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                                   &qpalm::QPALMSolver::get_info,
                                   py::return_value_policy::reference, py::keep_alive<0, 1>()))
        .def_property_readonly("prim_inf_certificate",
                               py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                                   &qpalm::QPALMSolver::get_prim_inf_certificate,
                                   py::return_value_policy::copy, py::keep_alive<0, 1>()))
        .def_property_readonly("dual_inf_certificate",
                               py::cpp_function( // https://github.com/pybind/pybind11/issues/2618
                                   &qpalm::QPALMSolver::get_dual_inf_certificate,
                                   py::return_value_policy::copy, py::keep_alive<0, 1>()))
        .def(
            "_get_c_work_ptr",
            [](qpalm::QPALMSolver &self) -> const void * { return self.get_c_work_ptr(); },
            "Return a pointer to the C workspace struct (of type ::QPALMWorkspace).",
            py::return_value_policy::reference_internal);
}

static int print_wrap(const char *fmt, ...) {
    static std::vector<char> buffer(1024);
    py::object write = py::module_::import("sys").attr("stdout").attr("write");
    std::va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);
    int needed = vsnprintf(buffer.data(), buffer.size(), fmt, args);
    va_end(args);
    // Error occurred
    if (needed < 0) {
        // ignore and return
    }
    // Buffer was too small
    else if (auto buf_needed = static_cast<size_t>(needed) + 1; buf_needed > buffer.size()) {
        buffer = std::vector<char>(buf_needed);
        va_start(args2, fmt);
        needed = vsnprintf(buffer.data(), buffer.size(), fmt, args2);
        va_end(args2);
    }
    if (needed >= 0)
        write(std::string_view{buffer.data(), static_cast<size_t>(needed)});
    return needed;
}
