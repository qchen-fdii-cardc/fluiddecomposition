#pragma once
#include <Eigen/Dense>
#include <vector>

class POD
{
public:
    POD() = default;

    // Compute POD modes from snapshot matrix X
    // X is a matrix where each column is a snapshot
    // k is the desired rank of the approximation
    void compute(const Eigen::MatrixXd &X, int k);

    // Project data onto POD modes
    Eigen::VectorXd project(const Eigen::VectorXd &data) const;

    // Reconstruct data from modal coefficients
    Eigen::VectorXd reconstruct(const Eigen::VectorXd &coeffs) const;

    // Get temporal coefficients for all snapshots
    Eigen::MatrixXd getTemporalCoefficients() const;

    // Get POD modes
    const Eigen::MatrixXd &modes() const { return modes_; }

    // Get singular values
    const Eigen::VectorXd &singularValues() const { return singular_values_; }

    // Get temporal modes (V matrix)
    const Eigen::MatrixXd &temporalModes() const { return temporal_modes_; }

private:
    Eigen::MatrixXd modes_;           // POD spatial modes (U)
    Eigen::VectorXd singular_values_; // Singular values (Σ)
    Eigen::MatrixXd temporal_modes_;  // Temporal modes (V)
    int rank_;                        // Rank of approximation
};