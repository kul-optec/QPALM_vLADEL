#include <qpalm/sparse.hpp>

#include <ladel_global.h> // ladel_sparse_free etc.

#include <algorithm> // copy_n
#include <cassert>

namespace qpalm {

ladel_sparse_matrix eigen_to_ladel(sparse_mat_t &mat, ladel_int symmetry) {
    ladel_sparse_matrix res{};
    res.nzmax    = mat.nonZeros();
    res.nrow     = mat.rows();
    res.ncol     = mat.cols();
    res.p        = mat.outerIndexPtr(); // column pointers
    res.i        = mat.innerIndexPtr(); // row indices
    res.x        = mat.valuePtr();
    res.nz       = mat.innerNonZeroPtr();
    res.values   = TRUE;
    res.symmetry = symmetry;
    return res;
}

namespace alloc {
void ladel_sparse_matrix_deleter::operator()(ladel_sparse_matrix *M) const {
    ::ladel_sparse_free(M);
}
} // namespace alloc

ladel_sparse_matrix_ptr ladel_sparse_create(index_t rows, index_t cols,
                                            index_t nnz, ladel_int symmetry,
                                            bool values, bool nonzeros) {
    return ladel_sparse_matrix_ptr{
        ::ladel_sparse_alloc(static_cast<ladel_int>(rows),
                             static_cast<ladel_int>(cols),
                             static_cast<ladel_int>(nnz), symmetry,
                             values ? TRUE : FALSE, nonzeros ? TRUE : FALSE),
    };
}

ladel_sparse_matrix_ptr eigen_to_ladel_copy(const sparse_mat_ref_t &mat,
                                            ladel_int symmetry) {
    bool nz  = mat.innerNonZeroPtr() != nullptr;
    auto res = ladel_sparse_create(mat.rows(), mat.cols(), mat.nonZeros(),
                                   symmetry, true, nz);
    assert(mat.outerSize() + 1 <= res->ncol + 1);
    std::copy_n(mat.outerIndexPtr(), mat.outerSize() + 1, res->p);
    assert(mat.nonZeros() <= res->nzmax);
    std::copy_n(mat.innerIndexPtr(), mat.nonZeros(), res->i);
    assert(mat.nonZeros() <= res->nzmax);
    std::copy_n(mat.valuePtr(), mat.nonZeros(), res->x);
    if (nz) {
        assert(mat.outerSize() <= res->ncol);
        std::copy_n(mat.innerNonZeroPtr(), mat.outerSize(), res->nz);
    }
    return res;
}

} // namespace qpalm