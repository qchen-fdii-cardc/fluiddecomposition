#pragma once
#include <Eigen/Dense>
#include <complex>
#include <vector>

class DMD {
public:
    DMD() = default;

    // Compute DMD modes from snapshot matrices X1 (x[0:n-1]) and X2 (x[1:n])
    // r is the desired rank of the approximation
    void compute(const Eigen::MatrixXd& X1, const Eigen::MatrixXd& X2, int r);

    // Compute DMD modes from a single snapshot matrix X
    // This will automatically create X1 and X2 by splitting X
    void compute(const Eigen::MatrixXd& X, int r);

    // Predict future state at time t
    Eigen::VectorXcd predict(double t) const;

    // Reconstruct the data at specified timesteps
    Eigen::MatrixXcd reconstruct(const Eigen::VectorXd& timesteps) const;

    // Getters for DMD components
    const Eigen::MatrixXcd& modes() const { return modes_; }
    const Eigen::VectorXcd& eigenvalues() const { return eigenvalues_; }
    const Eigen::VectorXcd& amplitudes() const { return amplitudes_; }
    double dt() const { return dt_; }

private:
    Eigen::MatrixXcd modes_;        // DMD modes (Phi)
    Eigen::VectorXcd eigenvalues_;  // DMD eigenvalues (Lambda)
    Eigen::VectorXcd amplitudes_;   // Mode amplitudes (b)
    double dt_;                     // Time step between snapshots
    int rank_;                      // Rank of approximation
}; 