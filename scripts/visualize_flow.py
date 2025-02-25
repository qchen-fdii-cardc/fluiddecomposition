import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import struct


def read_velocity_field(filename):
    with open(filename, 'rb') as f:
        # Read dimensions
        nx = struct.unpack('i', f.read(4))[0]
        ny = struct.unpack('i', f.read(4))[0]
        n_snapshots = struct.unpack('i', f.read(4))[0]

        # Read data
        data = np.frombuffer(f.read(), dtype=np.float64)
        data = data.reshape((2*nx*ny, n_snapshots))

        # Debug print
        print(f"\nReading {filename}:")
        print(f"Dimensions: nx={nx}, ny={ny}, snapshots={n_snapshots}")
        print(
            f"U range: {data[:nx*ny, :].min():.6f} to {data[:nx*ny, :].max():.6f}")
        print(
            f"V range: {data[nx*ny:, :].min():.6f} to {data[nx*ny:, :].max():.6f}")

        return data, nx, ny, n_snapshots


def read_complex_velocity_field(filename):
    with open(filename, 'rb') as f:
        # Read dimensions
        nx = struct.unpack('i', f.read(4))[0]
        ny = struct.unpack('i', f.read(4))[0]
        n_snapshots = struct.unpack('i', f.read(4))[0]

        # Calculate total size
        points_per_snapshot = 2 * nx * ny
        total_elements = points_per_snapshot * n_snapshots

        # Read data
        data = np.frombuffer(f.read(), dtype=np.float64)

        # Split into real and imaginary parts
        real_part = data[:total_elements].reshape(
            (points_per_snapshot, n_snapshots))
        imag_part = data[total_elements:].reshape(
            (points_per_snapshot, n_snapshots))

        # Debug print
        print(f"\nReading {filename}:")
        print(f"Dimensions: nx={nx}, ny={ny}, snapshots={n_snapshots}")
        print(
            f"U range (real): {real_part[:nx*ny, :].min():.6f} to {real_part[:nx*ny, :].max():.6f}")
        print(
            f"V range (real): {real_part[nx*ny:, :].min():.6f} to {real_part[nx*ny:, :].max():.6f}")

        # Combine into complex data
        complex_data = real_part + 1j * imag_part

        return complex_data, nx, ny, n_snapshots


def create_comparison_animation(pod_orig, pod_recon, dmd_orig, dmd_recon,
                                nx, ny, n_snapshots, output_filename):
    # Create figure with six subplots (2x3)
    fig, axes = plt.subplots(2, 3, figsize=(18, 10))

    # Create meshgrid for plotting
    x = np.linspace(0, 2*np.pi, nx)
    y = np.linspace(0, 2*np.pi, ny)
    X, Y = np.meshgrid(x, y)

    # Calculate global color limits for better comparison
    # For U component
    vmax_u = max(abs(pod_orig[:nx*ny, :].max()),
                 abs(pod_orig[:nx*ny, :].min()),
                 abs(pod_recon[:nx*ny, :].max()),
                 abs(pod_recon[:nx*ny, :].min()),
                 abs(dmd_recon[:nx*ny, :].real.max()),
                 abs(dmd_recon[:nx*ny, :].real.min()))

    # For V component
    vmax_v = max(abs(pod_orig[nx*ny:, :].max()),
                 abs(pod_orig[nx*ny:, :].min()),
                 abs(pod_recon[nx*ny:, :].max()),
                 abs(pod_recon[nx*ny:, :].min()),
                 abs(dmd_recon[nx*ny:, :].real.max()),
                 abs(dmd_recon[nx*ny:, :].real.min()))

    # Initialize plots with consistent colormap and scale
    plots = []
    for i in range(2):  # rows: u and v components
        for j in range(3):  # columns: original, POD, DMD
            plot = axes[i, j].pcolormesh(X, Y, np.zeros((ny, nx)),
                                         cmap='RdBu', shading='auto')
            plots.append(plot)
            axes[i, j].set_aspect('equal')
            axes[i, j].axis('off')

    # Unpack plots for easier reference
    u_pod_orig, v_pod_orig, u_pod_recon, v_pod_recon, u_dmd_recon, v_dmd_recon = plots

    # Set titles
    axes[0, 0].set_title('Original U')
    axes[1, 0].set_title('Original V')
    axes[0, 1].set_title('POD Reconstruction U')
    axes[1, 1].set_title('POD Reconstruction V')
    axes[0, 2].set_title('DMD Reconstruction U')
    axes[1, 2].set_title('DMD Reconstruction V')

    # Set color limits for each plot
    u_pod_orig.set_clim(-vmax_u, vmax_u)
    u_pod_recon.set_clim(-vmax_u, vmax_u)
    u_dmd_recon.set_clim(-vmax_u, vmax_u)
    v_pod_orig.set_clim(-vmax_v, vmax_v)
    v_pod_recon.set_clim(-vmax_v, vmax_v)
    v_dmd_recon.set_clim(-vmax_v, vmax_v)

    # Add colorbars with separate limits for U and V components
    for i, plot in enumerate(plots):
        if i < 3:  # U components
            plt.colorbar(
                plot, ax=axes[i//3, i % 3],
                label=f'U Velocity [-{vmax_u:.2f}, {vmax_u:.2f}]')
        else:  # V components
            plt.colorbar(
                plot, ax=axes[i//3, i % 3],
                label=f'V Velocity [-{vmax_v:.2f}, {vmax_v:.2f}]')

    def update(frame):
        # Extract POD components
        u_pod_orig_frame = pod_orig[:nx*ny, frame].reshape((ny, nx))
        v_pod_orig_frame = pod_orig[nx*ny:, frame].reshape((ny, nx))
        u_pod_recon_frame = pod_recon[:nx*ny, frame].reshape((ny, nx))
        v_pod_recon_frame = pod_recon[nx*ny:, frame].reshape((ny, nx))

        # Extract DMD components (take real part for visualization)
        u_dmd_orig_frame = dmd_orig[:nx*ny, frame].reshape((ny, nx)).real
        v_dmd_orig_frame = dmd_orig[nx*ny:, frame].reshape((ny, nx)).real
        u_dmd_recon_frame = dmd_recon[:nx*ny, frame].reshape((ny, nx)).real
        v_dmd_recon_frame = dmd_recon[nx*ny:, frame].reshape((ny, nx)).real

        # Update plots
        u_pod_orig.set_array(u_pod_orig_frame.ravel())
        v_pod_orig.set_array(v_pod_orig_frame.ravel())
        u_pod_recon.set_array(u_pod_recon_frame.ravel())
        v_pod_recon.set_array(v_pod_recon_frame.ravel())
        u_dmd_recon.set_array(u_dmd_recon_frame.ravel())
        v_dmd_recon.set_array(v_dmd_recon_frame.ravel())

        plt.suptitle(f'Time step: {frame}')
        return plots

    # Create animation
    anim = FuncAnimation(fig, update, frames=n_snapshots,
                         interval=100, blit=True)

    # Save animation
    anim.save(output_filename, writer='pillow', fps=10)
    plt.close()


if __name__ == "__main__":
    # Read POD data
    pod_orig, nx, ny, n_snapshots = read_velocity_field(
        "build/velocity_field_original.bin")
    pod_recon, _, _, _ = read_velocity_field(
        "build/velocity_field_reconstructed.bin")

    # Read DMD data
    dmd_orig, _, _, _ = read_complex_velocity_field(
        "build/velocity_field_original_dmd.bin")
    dmd_recon, _, _, _ = read_complex_velocity_field(
        "build/velocity_field_reconstructed_dmd.bin")

    # Print data statistics
    print("POD original stats:")
    print(f"Shape: {pod_orig.shape}")
    print(f"Min: {pod_orig.min()}, Max: {pod_orig.max()}")
    print("\nPOD reconstruction stats:")
    print(f"Shape: {pod_recon.shape}")
    print(f"Min: {pod_recon.min()}, Max: {pod_recon.max()}")

    print("\nDMD original stats:")
    print(f"Shape: {dmd_orig.shape}")
    print(f"Real min: {dmd_orig.real.min()}, Real max: {dmd_orig.real.max()}")
    print(f"Imag min: {dmd_orig.imag.min()}, Imag max: {dmd_orig.imag.max()}")
    print("\nDMD reconstruction stats:")
    print(f"Shape: {dmd_recon.shape}")
    print(
        f"Real min: {dmd_recon.real.min()}, Real max: {dmd_recon.real.max()}")
    print(
        f"Imag min: {dmd_recon.imag.min()}, Imag max: {dmd_recon.imag.max()}")

    # Print first few values
    print("\nFirst few values of DMD reconstruction:")
    print(dmd_recon[:5, :5])

    # Create comparison animation
    create_comparison_animation(pod_orig, pod_recon, dmd_orig, dmd_recon,
                                nx, ny, n_snapshots,
                                "doc/static/pod/velocity_field_comparison.gif")
