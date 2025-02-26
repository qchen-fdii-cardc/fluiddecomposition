import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def read_complex_csv(filename):
    """Read complex data from CSV file where each line has real,imag values"""
    data = np.loadtxt(filename, delimiter=',')
    return data[:, 0] + 1j * data[:, 1]


def plot_dmd_spectrum(eigenvalues, amplitudes):
    """
    Plot DMD spectrum with eigenvalues and amplitudes
    eigenvalues: complex eigenvalues
    amplitudes: complex amplitudes
    """
    # Create figure
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    # Get real and imaginary parts
    mu_real = eigenvalues.real
    mu_imag = eigenvalues.imag
    amp_abs = np.abs(amplitudes)

    # Plot eigenvalues as stems in 3D
    for x, y, z in zip(mu_real, mu_imag, amp_abs):
        ax.plot([x, x], [y, y], [0, z], 'b-', alpha=0.6)
        ax.scatter([x], [y], [z], c='red', s=50)

    # Plot unit circle on xy plane
    theta = np.linspace(0, 2*np.pi, 100)
    circle_x = np.cos(theta)
    circle_y = np.sin(theta)
    circle_z = np.zeros_like(theta)
    ax.plot(circle_x, circle_y, circle_z, 'k--',
            alpha=0.5, label='Unit Circle')

    # Customize plot
    ax.set_xlabel(r'Re($\mu$)', fontsize=12)
    ax.set_ylabel(r'Im($\mu$)', fontsize=12)
    ax.set_zlabel(r'$|b|$', fontsize=12)
    ax.set_title('DMD Spectrum', fontsize=14)

    # Set axis limits
    # max_val = max(max(abs(mu_real)), max(abs(mu_imag)), max(amp_abs))
    # ax.set_xlim([-1.5, 1.5])
    # ax.set_ylim([-1.5, 1.5])
    # ax.set_zlim([0, max_val*1.1])
    ax.axis('equal')

    # Add grid
    ax.grid(True, alpha=0.3)

    # Save plot
    plt.savefig('doc/static/pod/dmd_spectrum.png',
                dpi=300, bbox_inches='tight')
    plt.close()


if __name__ == "__main__":
    # Read eigenvalues and amplitudes from CSV files
    eigenvalues = read_complex_csv("build/dmd_results_eigenvalues.csv")
    amplitudes = read_complex_csv("build/dmd_results_amplitudes.csv")

    # Print data for verification
    print("Eigenvalues:")
    print(eigenvalues)
    print("\nAmplitudes:")
    print(amplitudes)

    # Create plot
    plot_dmd_spectrum(eigenvalues, amplitudes)
