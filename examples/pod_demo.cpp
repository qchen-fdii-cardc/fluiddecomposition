#include "pod.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>

const double M_PI = 3.14159265358979323846;

// Generate a simple test dataset with both u and v components
Eigen::MatrixXd generate_test_data(int nx, int ny, int n_snapshots)
{
    // Total number of spatial points for both u and v
    int n_points = 2 * nx * ny; // multiply by 2 for u and v components
    Eigen::MatrixXd X(n_points, n_snapshots);

    for (int t = 0; t < n_snapshots; ++t)
    {
        double time = t * 2.0 * M_PI / (n_snapshots - 1);

        // Fill u component (first half of the matrix)
        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {
                double x = i * 2.0 * M_PI / (nx - 1);
                double y = j * 2.0 * M_PI / (ny - 1);
                // u velocity component
                X(i + j * nx, t) = std::sin(x) * std::cos(y + time);
            }
        }

        // Fill v component (second half of the matrix)
        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {
                double x = i * 2.0 * M_PI / (nx - 1);
                double y = j * 2.0 * M_PI / (ny - 1);
                // v velocity component
                X(nx * ny + i + j * nx, t) = -std::cos(x) * std::sin(y + time);
            }
        }
    }
    return X;
}

// Compute and print reconstruction results for different ranks
void compare_ranks(const Eigen::MatrixXd &X, const std::vector<int> &ranks)
{
    POD pod;
    Eigen::VectorXd snapshot = X.col(0);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nComparing different ranks:\n";
    std::cout << std::setw(10) << "Rank"
              << std::setw(20) << "Error"
              << std::setw(20) << "Energy Captured" << "\n";
    std::cout << std::string(50, '-') << "\n";

    // First compute POD with maximum rank to get full singular value spectrum
    pod.compute(X, X.cols());
    double total_energy = pod.singularValues().squaredNorm();

    // Test each rank
    for (int rank : ranks)
    {
        pod.compute(X, rank);

        // Compute reconstruction
        Eigen::VectorXd coeffs = pod.project(snapshot);
        Eigen::VectorXd reconstruction = pod.reconstruct(coeffs);

        // Compute error and energy
        double error = (snapshot - reconstruction).norm() / snapshot.norm();
        double energy_ratio = pod.singularValues().head(rank).squaredNorm() / total_energy;

        std::cout << std::setw(10) << rank
                  << std::setw(20) << error
                  << std::setw(20) << energy_ratio * 100 << "%" << "\n";
    }
}

// Helper function to print velocity field components
void print_velocity_components(const Eigen::VectorXd &field, int nx, int ny)
{
    std::cout << "U component (first 5x5):\n";
    for (int j = 0; j < std::min(5, ny); ++j)
    {
        for (int i = 0; i < std::min(5, nx); ++i)
        {
            std::cout << std::setw(10) << field(i + j * nx);
        }
        std::cout << "\n";
    }

    std::cout << "\nV component (first 5x5):\n";
    for (int j = 0; j < std::min(5, ny); ++j)
    {
        for (int i = 0; i < std::min(5, nx); ++i)
        {
            std::cout << std::setw(10) << field(nx * ny + i + j * nx);
        }
        std::cout << "\n";
    }
}

// Add this function to save velocity field data
void save_velocity_field(const Eigen::MatrixXd &X, int nx, int ny, const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Save dimensions
    file.write(reinterpret_cast<const char *>(&nx), sizeof(nx));
    file.write(reinterpret_cast<const char *>(&ny), sizeof(ny));

    // Save number of snapshots
    int n_snapshots = X.cols();
    file.write(reinterpret_cast<const char *>(&n_snapshots), sizeof(n_snapshots));

    // Save data
    file.write(reinterpret_cast<const char *>(X.data()), X.size() * sizeof(double));

    file.close();
}

int main()
{
    // Generate test data
    const int nx = 32; // points in x direction
    const int ny = 32; // points in y direction
    const int n_snapshots = 50;
    Eigen::MatrixXd X = generate_test_data(nx, ny, n_snapshots);

    // Define ranks to compare
    std::vector<int> ranks = {1, 2, 3, 5, 10, 20};

    // Compare different ranks
    compare_ranks(X, ranks);

    // Plot singular value spectrum
    POD pod;
    pod.compute(X, X.cols());
    std::cout << "\nSingular Value Spectrum (first 10 values):\n";
    std::cout << pod.singularValues().head(10).transpose() << "\n";

    // Show detailed reconstruction for rank 3
    pod.compute(X, 3);
    Eigen::VectorXd snapshot = X.col(0);
    Eigen::VectorXd coeffs = pod.project(snapshot);
    Eigen::VectorXd reconstruction = pod.reconstruct(coeffs);

    std::cout << "\nDetailed comparison for rank 3:\n";
    std::cout << "Modal coefficients: " << coeffs.transpose() << "\n";

    // Print velocity field components
    std::cout << "\nOriginal velocity field:\n";
    print_velocity_components(snapshot, nx, ny);

    std::cout << "\nReconstructed velocity field:\n";
    print_velocity_components(reconstruction, nx, ny);

    // Show temporal evolution of modes
    std::cout << "\nTemporal coefficients for first 3 timesteps:\n";
    Eigen::MatrixXd temporal_coeffs = pod.getTemporalCoefficients();
    std::cout << temporal_coeffs.leftCols(3) << "\n";

    // Full reconstruction of snapshot matrix can be done as:
    // X ≈ U * Σ * Vᵀ = modes_ * temporal_coeffs
    Eigen::MatrixXd reconstruction_matrix = pod.modes() * temporal_coeffs;

    // Save the original velocity field data
    save_velocity_field(X, nx, ny, "build/velocity_field_original.bin");

    // Save the reconstructed velocity field data
    save_velocity_field(reconstruction_matrix, nx, ny, "build/velocity_field_reconstructed.bin");

    return 0;
}