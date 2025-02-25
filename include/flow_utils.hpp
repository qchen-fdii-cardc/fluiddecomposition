#pragma once
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <complex>

namespace flow {

// Generate test data for both POD and DMD
Eigen::MatrixXd generate_test_data(int nx, int ny, int n_snapshots, bool include_growth = false);

// Save real velocity field data
void save_velocity_field(const Eigen::MatrixXd& X, int nx, int ny, 
                        const std::string& filename);

// Save complex velocity field data
void save_complex_velocity_field(const Eigen::MatrixXcd& X, int nx, int ny,
                               const std::string& filename);

// Common parameters struct
struct FlowParams {
    static constexpr int nx = 32;
    static constexpr int ny = 32;
    static constexpr int n_snapshots = 50;
    static constexpr double pi = 3.14159265358979323846;
};

} // namespace flow 