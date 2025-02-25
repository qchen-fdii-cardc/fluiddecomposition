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

        return data, nx, ny, n_snapshots


def create_comparison_animation(orig_data, recon_data, nx, ny, n_snapshots, output_filename):
    # Create figure with four subplots
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))

    # Create meshgrid for plotting
    x = np.linspace(0, 2*np.pi, nx)
    y = np.linspace(0, 2*np.pi, ny)
    X, Y = np.meshgrid(x, y)

    # Initialize plots
    # Original velocity fields
    u_orig = ax1.pcolormesh(X, Y, np.zeros((ny, nx)),
                            cmap='RdBu', shading='auto')
    v_orig = ax2.pcolormesh(X, Y, np.zeros((ny, nx)),
                            cmap='RdBu', shading='auto')

    # Reconstructed velocity fields
    u_recon = ax3.pcolormesh(X, Y, np.zeros(
        (ny, nx)), cmap='RdBu', shading='auto')
    v_recon = ax4.pcolormesh(X, Y, np.zeros(
        (ny, nx)), cmap='RdBu', shading='auto')

    # Set titles
    ax1.set_title('Original U velocity')
    ax2.set_title('Original V velocity')
    ax3.set_title('Reconstructed U velocity')
    ax4.set_title('Reconstructed V velocity')

    # Add colorbars
    plt.colorbar(u_orig, ax=ax1)
    plt.colorbar(v_orig, ax=ax2)
    plt.colorbar(u_recon, ax=ax3)
    plt.colorbar(v_recon, ax=ax4)

    def update(frame):
        # Extract original U and V components
        u_orig_frame = orig_data[:nx*ny, frame].reshape((ny, nx))
        v_orig_frame = orig_data[nx*ny:, frame].reshape((ny, nx))

        # Extract reconstructed U and V components
        u_recon_frame = recon_data[:nx*ny, frame].reshape((ny, nx))
        v_recon_frame = recon_data[nx*ny:, frame].reshape((ny, nx))

        # Update plots
        u_orig.set_array(u_orig_frame.ravel())
        v_orig.set_array(v_orig_frame.ravel())
        u_recon.set_array(u_recon_frame.ravel())
        v_recon.set_array(v_recon_frame.ravel())

        # Update color scales
        vmin_u = min(orig_data[:nx*ny, :].min(), recon_data[:nx*ny, :].min())
        vmax_u = max(orig_data[:nx*ny, :].max(), recon_data[:nx*ny, :].max())
        vmin_v = min(orig_data[nx*ny:, :].min(), recon_data[nx*ny:, :].min())
        vmax_v = max(orig_data[nx*ny:, :].max(), recon_data[nx*ny:, :].max())

        u_orig.set_clim(vmin_u, vmax_u)
        u_recon.set_clim(vmin_u, vmax_u)
        v_orig.set_clim(vmin_v, vmax_v)
        v_recon.set_clim(vmin_v, vmax_v)

        plt.suptitle(f'Time step: {frame}')
        return u_orig, v_orig, u_recon, v_recon

    # Create animation
    anim = FuncAnimation(fig, update, frames=n_snapshots,
                         interval=100, blit=True)

    # Save animation
    anim.save(output_filename, writer='pillow', fps=10)
    plt.close()


if __name__ == "__main__":
    # Read original data
    orig_data, nx, ny, n_snapshots = read_velocity_field(
        "build/velocity_field_original.bin")

    # Read reconstructed data
    recon_data, _, _, _ = read_velocity_field(
        "build/velocity_field_reconstructed.bin")

    # Create comparison animation
    create_comparison_animation(orig_data, recon_data, nx, ny, n_snapshots,
                                "doc/static/pod/velocity_field_comparison.gif")
