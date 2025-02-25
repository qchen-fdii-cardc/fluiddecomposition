#include "flow_utils.hpp"
#include <cmath>
#include <stdexcept>

namespace flow {

Eigen::MatrixXd generate_test_data(int nx, int ny, int n_snapshots, bool include_growth)
{
    int n_points = 2 * nx * ny;
    Eigen::MatrixXd X(n_points, n_snapshots);

    for (int t = 0; t < n_snapshots; ++t)
    {
        double time = t * 2.0 * FlowParams::pi / (n_snapshots - 1);
        
        // Growth factors (only used if include_growth is true)
        double growth_factor = include_growth ? std::exp(0.1 * time) : 1.0;
        double decay_factor = include_growth ? std::exp(-0.05 * time) : 0.0;
        double scale = growth_factor + decay_factor;

        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {
                double x = i * 2.0 * FlowParams::pi / (nx - 1);
                double y = j * 2.0 * FlowParams::pi / (ny - 1);
                
                // u velocity component
                X(i + j * nx, t) = std::sin(x) * std::cos(y + time) * 
                                  (include_growth ? scale : 1.0);
                
                // v velocity component
                X(nx * ny + i + j * nx, t) = -std::cos(x) * std::sin(y + time) * 
                                            (include_growth ? scale : 1.0);
            }
        }
    }
    return X;
}

void save_velocity_field(const Eigen::MatrixXd& X, int nx, int ny, 
                        const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    file.write(reinterpret_cast<const char*>(&nx), sizeof(nx));
    file.write(reinterpret_cast<const char*>(&ny), sizeof(ny));
    
    int n_snapshots = X.cols();
    file.write(reinterpret_cast<const char*>(&n_snapshots), sizeof(n_snapshots));
    file.write(reinterpret_cast<const char*>(X.data()), X.size() * sizeof(double));
    
    file.close();
}

void save_complex_velocity_field(const Eigen::MatrixXcd& X, int nx, int ny,
                               const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    file.write(reinterpret_cast<const char*>(&nx), sizeof(nx));
    file.write(reinterpret_cast<const char*>(&ny), sizeof(ny));
    
    int n_snapshots = X.cols();
    file.write(reinterpret_cast<const char*>(&n_snapshots), sizeof(n_snapshots));
    
    Eigen::MatrixXd real_part = X.real();
    Eigen::MatrixXd imag_part = X.imag();
    
    file.write(reinterpret_cast<const char*>(real_part.data()),
               real_part.size() * sizeof(double));
    file.write(reinterpret_cast<const char*>(imag_part.data()),
               imag_part.size() * sizeof(double));
    
    file.close();
}

} // namespace flow 