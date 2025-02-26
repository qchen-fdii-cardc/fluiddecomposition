#pragma once
#include <Eigen/Dense>
#include <complex>
#include <vector>
#include <fstream>

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

    // Export results to CSV files
    void exportResults(const std::string& prefix) const {
        // Export modes (real and imaginary parts separately)
        exportComplexMatrix(modes_, prefix + "_modes");
        // Export eigenvalues
        exportComplexVector(eigenvalues_, prefix + "_eigenvalues.csv");
        // Export amplitudes
        exportComplexVector(amplitudes_, prefix + "_amplitudes.csv");
    }

private:
    Eigen::MatrixXcd modes_;        // DMD modes (Phi)
    Eigen::VectorXcd eigenvalues_;  // DMD eigenvalues (Lambda)
    Eigen::VectorXcd amplitudes_;   // Mode amplitudes (b)
    double dt_;                     // Time step between snapshots
    int rank_;                      // Rank of approximation

    // Helper function to export complex matrix
    static void exportComplexMatrix(const Eigen::MatrixXcd& mat, const std::string& prefix) {
        // Export real part
        std::ofstream real_file(prefix + "_real.csv");
        if (real_file.is_open()) {
            real_file << mat.real().format(Eigen::IOFormat(
                Eigen::FullPrecision,
                Eigen::DontAlignCols,
                ", ",
                "\n"
            ));
        }
        
        // Export imaginary part
        std::ofstream imag_file(prefix + "_imag.csv");
        if (imag_file.is_open()) {
            imag_file << mat.imag().format(Eigen::IOFormat(
                Eigen::FullPrecision,
                Eigen::DontAlignCols,
                ", ",
                "\n"
            ));
        }
    }

    // Helper function to export complex vector
    static void exportComplexVector(const Eigen::VectorXcd& vec, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (int i = 0; i < vec.size(); ++i) {
                file << vec(i).real() << ", " << vec(i).imag() << "\n";
            }
        }
    }
}; 