#include "pod.hpp"
#include <Eigen/SVD>

void POD::compute(const Eigen::MatrixXd &X, int k)
{
    // Check input validity
    if (k <= 0 || k > X.cols())
    {
        throw std::invalid_argument("Invalid rank k");
    }

    rank_ = k;

    // Compute SVD
    Eigen::BDCSVD<Eigen::MatrixXd> svd(X, Eigen::ComputeThinU | Eigen::ComputeThinV);

    // Store singular values
    singular_values_ = svd.singularValues();

    // Store first k POD modes (spatial modes)
    modes_ = svd.matrixU().leftCols(k);

    // Store first k temporal modes
    temporal_modes_ = svd.matrixV().leftCols(k);
}

Eigen::VectorXd POD::project(const Eigen::VectorXd &data) const
{
    if (data.size() != modes_.rows())
    {
        throw std::invalid_argument("Data dimension mismatch");
    }
    return modes_.transpose() * data;
}

Eigen::VectorXd POD::reconstruct(const Eigen::VectorXd &coeffs) const
{
    if (coeffs.size() != modes_.cols())
    {
        throw std::invalid_argument("Coefficient dimension mismatch");
    }
    return modes_ * coeffs;
}

Eigen::MatrixXd POD::getTemporalCoefficients() const
{
    // Return temporal coefficients: Σ * Vᵀ
    return (singular_values_.head(rank_).asDiagonal() * temporal_modes_.transpose());
}