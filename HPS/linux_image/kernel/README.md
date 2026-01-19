# Linux Kernel Build for DE10-Nano

This directory contains the automated kernel build system for DE10-Nano.

## Quick Start

```bash
# Build kernel (downloads source, configures, and builds)
cd HPS/kernel
make

# Or from repository root
cd HPS/kernel && make
```

## Build Targets

- `make` or `make all` - Build complete kernel (default)
- `make kernel-download` - Download/clone kernel source
- `make kernel-config` - Configure kernel
- `make kernel-build` - Build kernel (zImage, dtbs, modules)
- `make kernel-modules` - Build kernel modules only
- `make kernel-integrate-driver` - Integrate calculator driver
- `make kernel-clean` - Clean build artifacts
- `make kernel-distclean` - Remove kernel source and build

## Configuration

### Kernel Version

Set kernel version and branch in Makefile or via environment:

```bash
export KERNEL_BRANCH="socfpga-5.15.64-lts"
make kernel-build
```

### Cross-Compilation Toolchain

Default: `arm-linux-gnueabihf-`

Override:
```bash
make CROSS_COMPILE=arm-none-linux-gnueabihf- kernel-build
```

## Output Files

After build, kernel files are in `build/` directory:

- `build/arch/arm/boot/zImage` - Kernel image
- `build/arch/arm/boot/dts/socfpga_cyclone5_de10_nano.dtb` - Device tree blob
- `build/arch/arm/boot/dts/*.dtb` - Other device tree files
- `build/lib/modules/` - Kernel modules

## Driver Integration

The calculator driver is automatically integrated during build using:
- `HPS/integration/integrate_linux_driver.sh`

This adds the driver to the kernel build system.

## Custom Kernel Configuration

To customize kernel configuration:

```bash
cd linux-socfpga
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

Save configuration:
```bash
cp build/.config configs/my_custom_config
```

Use custom config:
```bash
cp configs/my_custom_config build/.config
make kernel-build
```

## Dependencies

Required tools:
- Cross-compilation toolchain (`arm-linux-gnueabihf-`)
- Git (for kernel source)
- Make, GCC, etc.
- Device tree compiler (`dtc`)

## Troubleshooting

### Kernel source download fails
- Check network connection
- Verify repository URL is accessible
- Try manual clone: `git clone -b socfpga-5.15.64-lts https://github.com/altera-opensource/linux-socfpga.git`

### Build fails
- Verify cross-compilation toolchain is installed
- Check disk space (kernel build requires ~5GB)
- Review build output for specific errors

### Device tree not found
- Device tree may be generated from FPGA build instead
- Check `FPGA/generated/soc_system.dtb`

## Integration with Rootfs Build

The kernel build is integrated with the rootfs build system. Kernel modules are automatically installed to rootfs during rootfs creation.
