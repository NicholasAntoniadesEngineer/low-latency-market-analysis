# DE10-Nano FPGA-Accelerated Market Analysis

Complete hardware-software platform for low-latency FPGA-accelerated computing on the Terasic DE10-Nano SoC board.

## Quick Start

```bash
# From repository root - build everything with parallelization
make everything    # FPGA + HPS build in parallel, then creates SD image

# Or build individual components
make fpga          # Build FPGA bitstream only
make kernel        # Build Linux kernel only
make rootfs        # Build root filesystem only
make sd-image      # Create SD card image (requires FPGA artifacts)
```

## Build System Features

| Feature | Description | Configuration |
|---------|-------------|---------------|
| **Parallel FPGA+HPS** | FPGA and HPS build simultaneously | `PARALLEL_EVERYTHING=1` |
| **Parallel Kernel+Rootfs** | Kernel and rootfs build simultaneously | `PARALLEL_BUILD=1` |
| **Quartus Parallelization** | Multi-core FPGA compilation | `QUARTUS_PARALLEL_JOBS=auto` |
| **ccache Support** | Faster kernel rebuilds | `USE_CCACHE=1` |
| **Tool Caching** | Cache Quartus/QSys paths | 60-minute cache |
| **Rootfs Base Caching** | Cache debootstrap base image | Rebuild only when packages.txt changes |
| **Build Timing** | Profile all build phases | `make timing-report` |
| **Incremental Updates** | Update existing SD images | `make sd-image-update` |

---

## Prerequisites

- **Hardware**: DE10-Nano board + MicroSD card (8GB+)
- **Software**: Quartus Prime Lite 20.1 + ARM cross-compiler
- **OS**: Windows with WSL2 or Linux environment
- **Optional**: DE10-Nano System CD (for prebuilt bootloaders), ccache

### Install Dependencies
```bash
make deps          # Install all build dependencies
sudo apt install ccache  # Optional: faster kernel rebuilds
```

---

## Build Commands

### Full Build (Recommended)
```bash
make everything          # Parallel FPGA + HPS build (~35-50 min)
make everything-parallel  # Force parallel mode
make everything-sequential # Force sequential mode (low memory)
```

### FPGA Build
```bash
make fpga              # Complete FPGA build (QSys + Quartus + RBF + DTB)
make fpga-qsys         # Generate QSys system only
make fpga-sof          # Compile FPGA bitstream
make fpga-rbf          # Convert to RBF format
make fpga-dtb          # Generate device tree from QSys
```

### HPS Build
```bash
make kernel            # Build Linux kernel (with ccache if available)
make rootfs            # Build root filesystem (uses base cache)
make applications      # Build HPS applications
make sd-image          # Create complete SD card image
make sd-image-update   # Incremental update (faster)
```

### Status and Diagnostics
```bash
make help              # Show all available targets
make status            # Show build artifact status
make timing-report     # Show build timing statistics
```

---

## Build Outputs

| Component | Output Location | Build Time |
|-----------|-----------------|------------|
| FPGA RBF | `FPGA/build/output_files/DE10_NANO_SoC_GHRD.rbf` | 15-25 min |
| Device Tree | `FPGA/generated/soc_system.dtb` | 1 min |
| Kernel | `HPS/linux_image/kernel/build/arch/arm/boot/zImage` | 10-20 min |
| Rootfs | `HPS/linux_image/rootfs/build/rootfs.tar.gz` | 15-25 min |
| SD Image | `HPS/linux_image/build/de10-nano-custom.img` | 2-5 min |

---

## Deploy to DE10-Nano

### Flash SD Card
```bash
# Find your SD card device
lsblk

# Flash the complete image (replace /dev/sdX)
sudo dd if=HPS/linux_image/build/de10-nano-custom.img of=/dev/sdX bs=4M status=progress conv=fsync
```

**Windows**: Use balenaEtcher, Win32DiskImager, or Rufus

### Boot & Connect
```bash
# SSH connect (default password: root)
ssh root@<board-ip>

# Run calculator tests
./calculator_test
```

### Verify System
```bash
cat /sys/class/fpga_manager/fpga0/state  # Should show "operating"
cat /sys/class/fpga_bridge/*/state       # Should show "enabled"
```

---

## Project Structure

```
Makefile              # Top-level orchestration (recommended entry point)
build/                # Common build infrastructure
├── build_common.mk   # Shared macros, timing, logging

FPGA/                 # Quartus FPGA design
├── Makefile          # FPGA build (QSys, Quartus, DTB)
├── qsys/             # QSys Platform Designer files
├── hdl/              # Verilog/VHDL source files
└── ip/               # Custom IP cores (calculator)

HPS/                  # Hard Processor System
├── Makefile          # HPS orchestration
├── linux_image/      # Linux build system
│   ├── Makefile      # Kernel + rootfs + SD image
│   ├── kernel/       # Kernel build with ccache support
│   └── rootfs/       # Rootfs build with base caching
├── applications/     # HPS applications (parallel build)
└── drivers/          # Linux driver integration

documentation/        # Build guides and references
```

---

## Configuration

### Parallelization Options
```bash
PARALLEL_EVERYTHING=1/0  # FPGA + HPS parallel (default: 1)
PARALLEL_BUILD=1/0       # Kernel + Rootfs parallel (default: 1)
PARALLEL_JOBS=N          # Parallel job count (default: 2)
QUARTUS_PARALLEL_JOBS=N  # Quartus jobs (default: auto-detect CPU count)
PARALLEL_APPS=1/0        # Applications parallel (default: 1)
```

### Performance Options
```bash
USE_CCACHE=1/0           # Enable ccache for kernel (default: 1 if available)
TOOL_CACHE_DISABLE=1     # Disable Quartus/QSys path caching
```

### Clean Operations
```bash
make clean               # Clean build artifacts (parallel, preserves caches)
make clean-all           # Deep clean including all caches
```

---

## Key Features

- **Parallel Build System**: FPGA and HPS build simultaneously
- **Intelligent Caching**: Tool paths, rootfs base, ccache for kernel
- **Build Profiling**: Timing reports for all build phases
- **Incremental Updates**: Only rebuild changed components
- **Hardware Acceleration**: FPGA-based floating-point calculator IP
- **Low-Latency Communication**: Direct HPS-FPGA memory-mapped I/O
- **Cross-Platform**: Windows/WSL/Linux compatibility

---

## Documentation

- **[Complete Deployment Guide](documentation/deployment_guide.md)** - Step-by-step build and troubleshooting
- **[FPGA-HPS Communication](documentation/hps_fpga_communication.md)** - Hardware interface details
- **[SoC EDS Setup](FPGA/SOC_EDS_SETUP.md)** - Intel SoC EDS configuration

---

## Getting Help

- Check the [Deployment Guide](documentation/deployment_guide.md) first
- Review Makefile help: `make help`
- For build issues: `make deps` to install dependencies
- Check kernel logs: `dmesg | grep fpga`

---

## References

### OEM Documentation
- [DE10-Nano CD Download](https://download.terasic.com/downloads/cd-rom/de10-nano/)
- [Terasic DE10-Nano](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=165&No=1046#contents)
- [Cyclone V HPS Register Address Map](https://www.intel.com/content/www/us/en/programmable/hps/cyclone-v/hps.html#sfo1418687413697.html)

### Hardware Manuals
- [DE10-Nano User Manual](documentation/references/DE10-Nano_User_manual_a_b.pdf)
- [Cyclone V Handbook](documentation/references/Cyclone_V_handbook.pdf)

### Community Resources
- [Building Embedded Linux for DE10-Nano](https://bitlog.it/20170820_building_embedded_linux_for_the_terasic_de10-nano.html)
- [zangman/de10-nano](https://github.com/zangman/de10-nano)

### Cornell University ECE5760
- [Linux Image](https://people.ece.cornell.edu/land/courses/ece5760/DE1_SOC/DE1-SoC-UP-Linux/linux_sdcard_image.zip)
- [FPGA Design](https://people.ece.cornell.edu/land/courses/ece5760/)
- [HPS Peripherals](https://people.ece.cornell.edu/land/courses/ece5760/DE1_SOC/HPS_peripherals/linux_index.html)