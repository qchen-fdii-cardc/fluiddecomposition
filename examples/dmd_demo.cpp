#include "dmd.hpp"
#include "flow_utils.hpp"
#include <iostream>
#include <iomanip>

int main()
{
    using namespace flow;

    // Generate test data with growth/decay modes
    Eigen::MatrixXd X = generate_test_data(FlowParams::nx, FlowParams::ny,
                                           FlowParams::n_snapshots, true);

    // Create DMD object and compute modes
    DMD dmd;
    const int rank = 4;
    dmd.compute(X, rank);

    // Print eigenvalues and first few entries of modes and amplitudes
    std::cout << "DMD eigenvalues:\n"
              << dmd.eigenvalues().head(rank).transpose() << "\n\n";

    std::cout << "First few DMD modes:\n"
              << dmd.modes().topRows(5) << "\n\n";

    std::cout << "Mode amplitudes:\n"
              << dmd.amplitudes().transpose() << "\n\n";

    // Generate prediction times
    Eigen::VectorXd predict_times(FlowParams::n_snapshots);
    for (int i = 0; i < FlowParams::n_snapshots; ++i)
    {
        predict_times(i) = i * 2.0 * FlowParams::pi / (FlowParams::n_snapshots - 1);
    }

    // Reconstruct the flow field
    Eigen::MatrixXcd reconstruction = dmd.reconstruct(predict_times);

    // Print some statistics about the reconstruction
    std::cout << "Reconstruction stats:\n"
              << "Min real: " << reconstruction.real().minCoeff() << "\n"
              << "Max real: " << reconstruction.real().maxCoeff() << "\n"
              << "Min imag: " << reconstruction.imag().minCoeff() << "\n"
              << "Max imag: " << reconstruction.imag().maxCoeff() << "\n"
              << "First few values:\n"
              << reconstruction.topLeftCorner(5, 5) << "\n\n";

    // Save the original and reconstructed data
    save_complex_velocity_field(X.cast<std::complex<double>>(), FlowParams::nx,
                                FlowParams::ny, "build/velocity_field_original_dmd.bin");
    save_complex_velocity_field(reconstruction, FlowParams::nx, FlowParams::ny,
                                "build/velocity_field_reconstructed_dmd.bin");

    return 0;
}