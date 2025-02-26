#pragma once
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <fstream>

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

    // Export results to CSV files
    void exportResults(const std::string& prefix) const {
        // Export modes
        exportMatrix(modes_, prefix + "_modes.csv");
        // Export singular values
        exportVector(singular_values_, prefix + "_singular_values.csv");
        // Export temporal coefficients
        exportMatrix(getTemporalCoefficients(), prefix + "_coefficients.csv");
    }

private:
    Eigen::MatrixXd modes_;           // POD spatial modes (U)
    Eigen::VectorXd singular_values_; // Singular values (Σ)
    Eigen::MatrixXd temporal_modes_;  // Temporal modes (V)
    int rank_;                        // Rank of approximation

    // Helper function to export matrix
    static void exportMatrix(const Eigen::MatrixXd& mat, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << mat.format(Eigen::IOFormat(
                Eigen::FullPrecision,
                Eigen::DontAlignCols,
                ", ",
                "\n"
            ));
        }
    }

    // Helper function to export vector
    static void exportVector(const Eigen::VectorXd& vec, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << vec.format(Eigen::IOFormat(
                Eigen::FullPrecision,
                Eigen::DontAlignCols,
                ", ",
                "\n"
            ));
        }
    }
};