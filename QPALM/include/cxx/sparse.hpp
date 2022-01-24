#pragma once

#include <Eigen/Sparse>            // Eigen::SparseMatrix
#include <Eigen/src/Core/Map.h>    // Eigen::Map
#include <Eigen/src/Core/Matrix.h> // Eigen::Matrix

#include <global_opts.h>     // c_float
#include <ladel_constants.h> // UNSYMMETRIC
#include <ladel_types.h>     // ladel_sparse_matrix

#include <memory> // unique_ptr

namespace qpalm {

/// Index types for vectors and matrices.
using index_t = Eigen::Index;
/// Index type for sparse matrices representation.
using sp_index_t = ladel_int;
/// Owning sparse matrix type.
using sparse_mat_t = Eigen::SparseMatrix<c_float, Eigen::ColMajor, sp_index_t>;
/// Owning dense vector type.
using vec_t = Eigen::Matrix<c_float, Eigen::Dynamic, 1>;
/// Borrowed dense vector type (vector view).
using borrowed_vec_t = Eigen::Map<vec_t>;
/// Read-only borrowed dense vector type (vector view).
using const_borrowed_vec_t = Eigen::Map<const vec_t>;

namespace alloc {
struct ladel_sparse_matrix_deleter {
    void operator()(ladel_sparse_matrix *) const;
};
} // namespace alloc

/// Smart pointer that automatically cleans up an owning ladel_sparse_matrix
/// object.
using ladel_sparse_matrix_ptr =
    std::unique_ptr<ladel_sparse_matrix, alloc::ladel_sparse_matrix_deleter>;

/// Convert an Eigen sparse matrix to a LADEL sparse matrix, without creating
/// a copy.
/// @note   The returned object contains pointers to the data of @p mat, so do 
///         not reallocate or deallocate using the @c ladel_sparse_free 
///         and similar functions. Modifications of the returned LADEL matrix
///         will affect the original Eigen matrix, so make sure that the 
///         representation remains consistent.
inline ladel_sparse_matrix eigen_to_ladel(sparse_mat_t &mat,
                                          ladel_int symmetry = UNSYMMETRIC);

/// Similar to @ref eigen_to_ladel, but creates a copy of all data, in such a
/// way that the returned matrix is completely decoupled from @ref mat, and such
/// that it can be reallocated and deallocated by the @ref ladel_sparse_free
/// and similar functions.
ladel_sparse_matrix_ptr eigen_to_ladel_copy(const sparse_mat_t &mat);

} // namespace qpalm