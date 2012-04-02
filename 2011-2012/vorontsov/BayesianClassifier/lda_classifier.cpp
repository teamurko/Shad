#include "lda_classifier.h"

CovarianceMatrix inversed(CovarianceMatrix matrix)
{
    bnu::permutation_matrix<> permutation_matrix(matrix.size1());
    bnu::lu_factorize(matrix, permutation_matrix);
    CovarianceMatrix inversed(bnu::identity_matrix<double>(matrix.size1()));
    bnu::lu_substitute(matrix, permutation_matrix, inversed);
    return inversed;
}

