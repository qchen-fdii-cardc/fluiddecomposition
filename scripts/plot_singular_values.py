import numpy as np
import matplotlib.pyplot as plt


def plot_singular_values():
    # Create data points
    n_points = 20
    x = np.arange(1, n_points + 1)

    # Generate two decay curves
    # POD: exponential decay to zero
    to_zero = np.exp(-0.3 * x)

    # DMD: exponential decay to a constant
    constant = 0.1
    to_constant = np.exp(-0.2 * x) + constant

    # Create figure
    plt.figure(figsize=(10, 6))

    # Plot curves
    plt.plot(x, to_zero, 'b-o', label='converge to zero', markersize=6)
    plt.plot(x, to_constant, 'r-s', label='converge to constant', markersize=6)

    # Customize plot
    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.xlabel(r'$\tau$', fontsize=12)
    plt.ylabel(r'$\sigma_i$', fontsize=12)
    plt.title(r'$\sigma_i(\tau)$ for SVD', fontsize=14)
    plt.legend(fontsize=10)

    # Add horizontal line at constant value for DMD
    plt.axhline(y=constant, color='gray', linestyle='--', alpha=0.5)
    plt.text(n_points/2, constant*1.5, f'Asymptotic Value ≈ {constant}',
             horizontalalignment='center', color='gray')

    # Save plot
    plt.savefig('doc/static/pod/singular_values_comparison.png',
                dpi=300, bbox_inches='tight')
    plt.close()


if __name__ == "__main__":
    plot_singular_values()
