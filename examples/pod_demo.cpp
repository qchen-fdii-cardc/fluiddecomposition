#include "pod.hpp"
#include "flow_utils.hpp"
#include <iostream>
#include <iomanip>

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

int main()
{
    using namespace flow;

    // Generate test data
    Eigen::MatrixXd X = flow::generate_test_data(FlowParams::nx, FlowParams::ny,
                                                FlowParams::n_snapshots, false);

    // Create POD object and compute modes
    POD pod;
    const int rank = 4;
    pod.compute(X, rank);

    // Print singular values
    std::cout << "Singular values:\n"
              << pod.singularValues().head(rank).transpose() << "\n\n";

    // Reconstruct the flow field
    Eigen::MatrixXd reconstruction_matrix = pod.modes() * pod.getTemporalCoefficients();

    // Save the original and reconstructed velocity field data
    flow::save_velocity_field(X, FlowParams::nx, FlowParams::ny,
                            "build/velocity_field_original.bin");
    flow::save_velocity_field(reconstruction_matrix, FlowParams::nx, FlowParams::ny,
                            "build/velocity_field_reconstructed.bin");

    return 0;
}