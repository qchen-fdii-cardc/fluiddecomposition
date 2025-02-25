#include "dmd.hpp"
#include <Eigen/SVD>

void DMD::compute(const Eigen::MatrixXd &X1, const Eigen::MatrixXd &X2, int r)
{
    if (r <= 0 || r > X1.cols())
    {
        throw std::invalid_argument("Invalid rank r");
    }
    rank_ = r;

    // Compute SVD of X1
    Eigen::BDCSVD<Eigen::MatrixXd> svd(X1, Eigen::ComputeThinU | Eigen::ComputeThinV);

    // Truncate to rank r
    Eigen::MatrixXd U = svd.matrixU().leftCols(r);
    Eigen::VectorXd sigma = svd.singularValues().head(r);
    Eigen::MatrixXd V = svd.matrixV().leftCols(r);

    // Compute reduced system matrix S = U^T * X2 * V * Sigma^(-1)
    Eigen::MatrixXd S = U.transpose() * X2 * V * sigma.asDiagonal().inverse();

    // Eigendecomposition of S
    Eigen::EigenSolver<Eigen::MatrixXd> eig(S);
    eigenvalues_ = eig.eigenvalues();

    // Compute DMD modes (Phi = U * W)
    modes_ = U * eig.eigenvectors();

    // Compute amplitudes by solving Phi * b = x1
    // Convert to complex before solving to handle complex modes
    Eigen::MatrixXcd modes_complex = modes_.cast<std::complex<double>>();
    amplitudes_ = modes_complex.colPivHouseholderQr().solve(X1.col(0).cast<std::complex<double>>());

    // Estimate dt based on the assumption that time steps are 1 unit apart
    dt_ = 1.0;

    // Compute eigenvalues of A (continuous time)
    eigenvalues_ = (eigenvalues_.array().log()) / dt_;
}

void DMD::compute(const Eigen::MatrixXd &X, int r)
{
    // Split X into X1 and X2
    Eigen::MatrixXd X1 = X.leftCols(X.cols() - 1);
    Eigen::MatrixXd X2 = X.rightCols(X.cols() - 1);
    compute(X1, X2, r);
}

Eigen::VectorXcd DMD::predict(double t) const
{
    // Continuous time prediction
    Eigen::VectorXcd timeEvolution = (eigenvalues_.array() * t).exp();
    return modes_ * (amplitudes_.array() * timeEvolution.array()).matrix();
}

Eigen::MatrixXcd DMD::reconstruct(const Eigen::VectorXd &timesteps) const
{
    Eigen::MatrixXcd reconstruction(modes_.rows(), timesteps.size());
    for (int i = 0; i < timesteps.size(); ++i)
    {
        reconstruction.col(i) = predict(timesteps(i));
    }
    return reconstruction;
}