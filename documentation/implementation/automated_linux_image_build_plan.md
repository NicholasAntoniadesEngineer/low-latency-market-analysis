# Automated Linux Image Build Implementation Plan

**Goal:** Automate building complete Linux image for DE10-Nano with:
- FPGA drivers (calculator driver) integrated
- Ethernet/SSH pre-configured
- Easy deployment via scripts

**Target:** Single command deployment: `./Scripts/build_and_deploy_image.sh`

---

## Overview

This plan implements a complete automated build system for creating a bootable Linux SD card image with all required components:

1. **Kernel Build** - Automated with FPGA drivers
2. **Root Filesystem** - Pre-configured with network and SSH
3. **Complete SD Card Image** - Bootable image creation
4. **Deployment Scripts** - Easy one-command deployment

---

## Phase 1: Kernel Build Automation

### 1.1 Create Kernel Build Structure

**Directory:** `HPS/kernel/`

**Files to Create:**
- `HPS/kernel/Makefile` - Main kernel build automation
- `HPS/kernel/configs/socfpga_defconfig` - Kernel configuration
- `HPS/kernel/patches/` - Directory for kernel patches
- `HPS/kernel/README.md` - Kernel build documentation

### 1.2 Kernel Build Makefile

**Location:** `HPS/kernel/Makefile`

**Features:**
- Automatic kernel source download/clone
- Version management (git tag/branch)
- Configuration management
- Driver integration (calculator driver)
- Cross-compilation support
- Module build support

**Key Targets:**
```makefile
kernel-download    # Clone/download kernel source
kernel-config      # Generate/update kernel config
kernel-build       # Build kernel (zImage, dtbs, modules)
kernel-modules     # Build kernel modules only
kernel-clean       # Clean build artifacts
```

### 1.3 Driver Integration

**Calculator Driver Integration:**
- Use existing `HPS/integration/integrate_linux_driver.sh`
- Automate driver integration into kernel build
- Add device tree entries for calculator IP
- Support both kernel module and userspace drivers

**Device Tree Integration:**
- Automatically merge calculator device tree entries
- Update generated DTS from FPGA build
- Ensure compatibility with kernel device tree

### 1.4 Kernel Configuration

**Default Config:** `HPS/kernel/configs/socfpga_defconfig`

**Required Options:**
- FPGA manager support
- UIO support (for advanced examples)
- Network support (Ethernet drivers)
- SSH/network tools support
- Calculator driver (if kernel module)

---

## Phase 2: Root Filesystem Build Automation

### 2.1 Create Rootfs Build Structure

**Directory:** `HPS/rootfs/`

**Files to Create:**
- `HPS/rootfs/build_rootfs.sh` - Main rootfs build script
- `HPS/rootfs/packages.txt` - Package list
- `HPS/rootfs/configs/` - Configuration templates
  - `network/interfaces` - Network configuration
  - `ssh/sshd_config` - SSH server config
  - `systemd/` - Systemd service files
- `HPS/rootfs/scripts/` - Post-install scripts
- `HPS/rootfs/Makefile` - Rootfs build automation

### 2.2 Rootfs Build Script

**Location:** `HPS/rootfs/build_rootfs.sh`

**Features:**
- Debootstrap-based Debian rootfs creation
- Package installation from `packages.txt`
- Network configuration (DHCP or static)
- SSH server installation and configuration
- User account setup
- FPGA driver installation
- Custom service setup

**Process:**
1. Create rootfs directory structure
2. Run debootstrap to create base Debian system
3. Install packages from list
4. Copy configuration files
5. Run post-install scripts
6. Create rootfs tarball or image

### 2.3 Network Configuration

**File:** `HPS/rootfs/configs/network/interfaces`

**Configuration Options:**
- **DHCP Mode (Default):**
  ```bash
  auto eth0
  iface eth0 inet dhcp
  ```

- **Static IP Mode (Optional):**
  ```bash
  auto eth0
  iface eth0 inet static
      address 192.168.1.100
      netmask 255.255.255.0
      gateway 192.168.1.1
  ```

### 2.4 SSH Configuration

**File:** `HPS/rootfs/configs/ssh/sshd_config`

**Features:**
- SSH server enabled by default
- Root login allowed (for development)
- Password authentication enabled
- Default port 22

**Post-Install:**
- Install openssh-server package
- Enable SSH service
- Set root password (configurable)

### 2.5 Package List

**File:** `HPS/rootfs/packages.txt`

**Essential Packages:**
```
openssh-server
net-tools
iputils-ping
curl
wget
build-essential
python3
vim
nano
htop
```

**FPGA-Related:**
```
uio-module-drv
device-tree-compiler
```

### 2.6 Post-Install Scripts

**Directory:** `HPS/rootfs/scripts/`

**Scripts:**
- `setup_network.sh` - Configure network
- `setup_ssh.sh` - Configure SSH
- `install_fpga_drivers.sh` - Install FPGA drivers
- `setup_services.sh` - Enable system services

---

## Phase 3: Complete SD Card Image Creation

### 3.1 Image Creation Script

**Location:** `HPS/create_sd_image.sh`

**Features:**
- Create complete bootable SD card image
- Partition table (MBR)
- FAT32 boot partition
- ext4 rootfs partition
- Copy all boot files
- Extract rootfs
- Generate image file

### 3.2 Partition Layout

**Standard Layout:**
```
Partition 1: FAT32, 100MB (boot)
  - preloader-mkpimage.bin (flashed to partition 3)
  - u-boot.img
  - zImage
  - soc_system.dtb
  - soc_system.rbf

Partition 2: ext4, remaining space (rootfs)
  - Complete root filesystem

Partition 3: Raw, 1MB (preloader - not mounted)
  - preloader-mkpimage.bin (flashed here)
```

### 3.3 Image Creation Process

1. **Create Image File:**
   ```bash
   dd if=/dev/zero of=de10-nano-image.img bs=1M count=4096  # 4GB image
   ```

2. **Create Partition Table:**
   ```bash
   parted de10-nano-image.img --script mklabel msdos
   parted de10-nano-image.img --script mkpart primary fat32 1MiB 101MiB
   parted de10-nano-image.img --script mkpart primary ext4 101MiB 100%
   ```

3. **Setup Loop Device:**
   ```bash
   losetup -P /dev/loop0 de10-nano-image.img
   ```

4. **Format Partitions:**
   ```bash
   mkfs.vfat -F 32 /dev/loop0p1
   mkfs.ext4 /dev/loop0p2
   ```

5. **Copy Boot Files:**
   ```bash
   mount /dev/loop0p1 /mnt/boot
   cp preloader-mkpimage.bin /mnt/boot/
   cp u-boot.img /mnt/boot/
   cp zImage /mnt/boot/
   cp soc_system.dtb /mnt/boot/
   cp soc_system.rbf /mnt/boot/
   umount /mnt/boot
   ```

6. **Extract Rootfs:**
   ```bash
   mount /dev/loop0p2 /mnt/rootfs
   tar -xzf rootfs.tar.gz -C /mnt/rootfs
   umount /mnt/rootfs
   ```

7. **Flash Preloader:**
   ```bash
   dd if=preloader-mkpimage.bin of=/dev/loop0 bs=64k seek=0
   ```

8. **Cleanup:**
   ```bash
   losetup -d /dev/loop0
   ```

---

## Phase 4: Unified Build System

### 4.1 Main Build Script

**Location:** `Scripts/build_linux_image.sh`

**Features:**
- Orchestrates entire build process
- Dependency management
- Progress reporting
- Error handling
- Configuration management

**Workflow:**
1. Build FPGA bitstream (if needed)
2. Build preloader and U-Boot
3. Generate device tree
4. Build kernel with drivers
5. Build rootfs
6. Create SD card image

### 4.2 Configuration File

**Location:** `HPS/build_config.sh` or `HPS/build_config.conf`

**Configuration Options:**
```bash
# Kernel
KERNEL_VERSION="5.15.0"
KERNEL_REPO="https://github.com/altera-opensource/linux-socfpga.git"
KERNEL_BRANCH="socfpga-5.15.64-lts"

# Rootfs
ROOTFS_DISTRO="debian"
ROOTFS_VERSION="bullseye"
ROOTFS_ARCH="armhf"

# Network
NETWORK_MODE="dhcp"  # or "static"
STATIC_IP="192.168.1.100"
STATIC_GATEWAY="192.168.1.1"
STATIC_NETMASK="255.255.255.0"

# SSH
SSH_ENABLED="yes"
SSH_ROOT_LOGIN="yes"
ROOT_PASSWORD="root"  # Change after first boot

# Image
IMAGE_SIZE="4096"  # MB
IMAGE_NAME="de10-nano-custom.img"
```

### 4.3 Makefile Integration

**Extend:** `HPS/Makefile`

**New Targets:**
```makefile
linux-image       # Build complete Linux image
kernel            # Build kernel only
rootfs            # Build rootfs only
sd-image          # Create SD card image
deploy-image      # Build and deploy to SD card
```

---

## Phase 5: Deployment Scripts

### 5.1 Deployment Script

**Location:** `Scripts/deploy_image.sh`

**Features:**
- Build complete image
- Write to SD card
- Verify image
- Optional: Network deployment

**Usage:**
```bash
./Scripts/deploy_image.sh /dev/sdX
```

### 5.2 Network Deployment

**Location:** `Scripts/deploy_via_network.sh`

**Features:**
- Deploy to running board via network
- Update boot files on SD card
- Update rootfs (if needed)
- Reboot board

---

## Phase 6: Driver Integration Details

### 6.1 Calculator Driver Integration

**Automated Integration:**
- Use `HPS/integration/integrate_linux_driver.sh`
- Integrate during kernel build
- Add to kernel config automatically
- Update device tree automatically

### 6.2 Device Tree Updates

**Process:**
1. Generate device tree from FPGA build
2. Add calculator driver entries
3. Merge with kernel device tree
4. Compile final DTB

**Device Tree Entry:**
```dts
calculator_0: calculator@ff280000 {
    compatible = "altr,calculator-1.1";
    reg = <0xff280000 0x40>;
    status = "okay";
};
```

### 6.3 Kernel Module vs Userspace

**Options:**
- **Kernel Module:** Built into kernel or as loadable module
- **Userspace:** Use existing `/dev/mem` approach (current)

**Recommendation:** Start with userspace (simpler), add kernel module option later.

---

## Implementation Steps

### Step 1: Kernel Build Automation (Priority 1)

1. Create `HPS/kernel/` directory
2. Create `HPS/kernel/Makefile` with build targets
3. Add kernel source management
4. Integrate driver integration script
5. Test kernel build

**Files:**
- `HPS/kernel/Makefile`
- `HPS/kernel/configs/socfpga_defconfig`
- `HPS/kernel/README.md`

### Step 2: Rootfs Build Automation (Priority 2)

1. Create `HPS/rootfs/` directory
2. Create `HPS/rootfs/build_rootfs.sh`
3. Create package list
4. Create configuration templates
5. Create post-install scripts
6. Test rootfs build

**Files:**
- `HPS/rootfs/build_rootfs.sh`
- `HPS/rootfs/packages.txt`
- `HPS/rootfs/configs/network/interfaces`
- `HPS/rootfs/configs/ssh/sshd_config`
- `HPS/rootfs/scripts/` (multiple scripts)

### Step 3: SD Card Image Creation (Priority 3)

1. Create `HPS/create_sd_image.sh`
2. Integrate with build system
3. Test image creation
4. Test booting from image

**Files:**
- `HPS/create_sd_image.sh`
- `HPS/Makefile` (extend with new targets)

### Step 4: Unified Build System (Priority 4)

1. Create `Scripts/build_linux_image.sh`
2. Create configuration file
3. Integrate all components
4. Add error handling and validation
5. Test complete build

**Files:**
- `Scripts/build_linux_image.sh`
- `HPS/build_config.sh`

### Step 5: Deployment Scripts (Priority 5)

1. Create `Scripts/deploy_image.sh`
2. Create `Scripts/deploy_via_network.sh`
3. Add verification and testing
4. Document usage

**Files:**
- `Scripts/deploy_image.sh`
- `Scripts/deploy_via_network.sh`

---

## File Structure

```
HPS/
├── kernel/
│   ├── Makefile
│   ├── configs/
│   │   └── socfpga_defconfig
│   ├── patches/
│   └── README.md
├── rootfs/
│   ├── build_rootfs.sh
│   ├── packages.txt
│   ├── configs/
│   │   ├── network/
│   │   │   └── interfaces
│   │   └── ssh/
│   │       └── sshd_config
│   ├── scripts/
│   │   ├── setup_network.sh
│   │   ├── setup_ssh.sh
│   │   ├── install_fpga_drivers.sh
│   │   └── setup_services.sh
│   └── Makefile
├── create_sd_image.sh
├── build_config.sh
└── Makefile (extended)

Scripts/
├── build_linux_image.sh
├── deploy_image.sh
└── deploy_via_network.sh
```

---

## Usage Examples

### Build Complete Image

```bash
# From repository root
./Scripts/build_linux_image.sh

# Or using Makefile
cd HPS
make linux-image
```

### Deploy to SD Card

```bash
# Build and deploy in one command
./Scripts/deploy_image.sh /dev/sdX

# Or build first, then deploy
./Scripts/build_linux_image.sh
./Scripts/deploy_image.sh /dev/sdX
```

### Build Individual Components

```bash
# Build kernel only
cd HPS/kernel
make kernel-build

# Build rootfs only
cd HPS/rootfs
./build_rootfs.sh

# Create SD image from existing components
cd HPS
./create_sd_image.sh
```

---

## Configuration

### Network Configuration

**DHCP (Default):**
```bash
# In HPS/build_config.sh
NETWORK_MODE="dhcp"
```

**Static IP:**
```bash
# In HPS/build_config.sh
NETWORK_MODE="static"
STATIC_IP="192.168.1.100"
STATIC_GATEWAY="192.168.1.1"
STATIC_NETMASK="255.255.255.0"
```

### SSH Configuration

**Enable SSH:**
```bash
# In HPS/build_config.sh
SSH_ENABLED="yes"
SSH_ROOT_LOGIN="yes"
ROOT_PASSWORD="root"  # Change after first boot!
```

---

## Testing

### Test Kernel Build

```bash
cd HPS/kernel
make kernel-build
# Verify: zImage exists in kernel build directory
```

### Test Rootfs Build

```bash
cd HPS/rootfs
./build_rootfs.sh
# Verify: rootfs.tar.gz created
# Test: Mount and verify contents
```

### Test Image Creation

```bash
cd HPS
./create_sd_image.sh
# Verify: de10-nano-custom.img created
# Test: Write to SD card and boot
```

### Test Complete Build

```bash
./Scripts/build_linux_image.sh
# Verify: All components built
# Test: Boot from SD card
# Test: SSH connection
# Test: FPGA driver functionality
```

---

## Dependencies

### Required Tools

1. **Kernel Build:**
   - Cross-compilation toolchain (`arm-linux-gnueabihf-`)
   - Git (for kernel source)
   - Make, GCC, etc.

2. **Rootfs Build:**
   - `debootstrap`
   - `qemu-user-static` (for ARM chroot)
   - Root access (for chroot operations)

3. **Image Creation:**
   - `parted` or `fdisk`
   - `mkfs.vfat`, `mkfs.ext4`
   - `losetup` (Linux)
   - `dd`

### Verification Scripts

Create scripts to verify all dependencies are installed:
- `Scripts/check_dependencies.sh`

---

## Error Handling

### Build Failures

- Check dependency installation
- Verify toolchain availability
- Check disk space
- Verify network connectivity (for downloads)

### Image Creation Failures

- Check root access
- Verify loop device availability
- Check disk space
- Verify partition tools

### Deployment Failures

- Verify SD card device
- Check write permissions
- Verify image file integrity

---

## Documentation

### User Documentation

- `HPS/kernel/README.md` - Kernel build guide
- `HPS/rootfs/README.md` - Rootfs build guide
- `Scripts/README.md` - Deployment guide

### Developer Documentation

- Code comments
- Configuration file documentation
- Troubleshooting guide

---

## Timeline Estimate

- **Phase 1 (Kernel):** 2-3 days
- **Phase 2 (Rootfs):** 2-3 days
- **Phase 3 (Image):** 1-2 days
- **Phase 4 (Integration):** 1-2 days
- **Phase 5 (Deployment):** 1 day
- **Testing & Documentation:** 1-2 days

**Total:** ~1-2 weeks

---

## Success Criteria

1. ✅ Single command builds complete Linux image
2. ✅ Image includes FPGA drivers (calculator)
3. ✅ Ethernet/SSH pre-configured and working
4. ✅ Image boots successfully on DE10-Nano
5. ✅ SSH connection works out of the box
6. ✅ FPGA drivers functional
7. ✅ Easy deployment to SD card
8. ✅ Well-documented process

---

## Next Steps

1. Review and approve this plan
2. Start with Phase 1 (Kernel Build Automation)
3. Iterate and test each phase
4. Integrate with existing build system
5. Document and test complete workflow
