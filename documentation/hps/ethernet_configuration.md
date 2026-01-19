# Ethernet Configuration for DE10-Nano

## Overview

The DE10-Nano has **Ethernet built into the HPS (Hard Processor System)**, not in the FPGA fabric. For basic internet access, you only need the HPS Ethernet interface configured - **no FPGA implementation is required**.

## Architecture

### HPS Ethernet (GMAC)

The DE10-Nano SoC includes a **Gigabit Ethernet MAC (GMAC)** controller in the HPS:
- **Hardware**: Built into the ARM Cortex-A9 processor subsystem
- **Address**: `0xFF702000` (GMAC1)
- **Interface**: RGMII (Reduced Gigabit Media Independent Interface)
- **Physical Connection**: Connected to the Ethernet RJ-45 port on the board

### FPGA Role

The FPGA **does NOT implement Ethernet**. It only:
- Passes through HPS I/O pins to the physical connector
- The Ethernet signals are routed through the FPGA fabric but not processed by it

## Current Configuration Status

### ✅ Hardware Level (Already Configured)

The Ethernet hardware is already configured in the FPGA design:

**File:** `FPGA/hdl/DE10_NANO_SoC_GHRD.v` (lines 143-157)
```verilog
//HPS ethernet
.hps_0_hps_io_hps_io_emac1_inst_TX_CLK(HPS_ENET_GTX_CLK),
.hps_0_hps_io_hps_io_emac1_inst_TXD0(HPS_ENET_TX_DATA[0]),
// ... (all Ethernet pins connected)
```

The HPS Ethernet pins are connected to the board's physical Ethernet connector.

### ✅ Device Tree Level (Already Configured)

The device tree configuration is already set up:

**File:** `FPGA/quartus/qsys/hps_common_board_info.xml`
- Ethernet alias: `ethernet0` → `/sopc@0/ethernet@0xff702000`
- GMAC configuration: RGMII mode, PHY address, timing skews
- Reset configuration for GMAC

This configuration is automatically included when generating the device tree with `make dts`.

### ⚠️ Kernel Level (Needs Verification)

The Linux kernel needs the **STMMAC Ethernet driver** enabled:

**Required Kernel Options:**
```
CONFIG_STMMAC_ETH=y          # STMMAC Ethernet driver
CONFIG_DWMAC_GENERIC=y       # Generic DWMAC support
CONFIG_DWMAC_SOCFPGA=y       # SoCFPGA-specific support
```

**Default socfpga_defconfig** should include these, but verify during kernel build.

### ✅ Software Level (Already Configured)

The rootfs build system already configures the network interface:

**File:** `HPS/rootfs/configs/network/interfaces`
```bash
auto eth0
iface eth0 inet dhcp
```

This is automatically installed during rootfs creation.

## What You Need to Do

### For Basic Internet Access:

**Nothing additional is required!** The system is already configured:

1. ✅ **Hardware**: HPS Ethernet pins connected (in FPGA HDL)
2. ✅ **Device Tree**: GMAC configured (in board info files)
3. ✅ **Software**: Network interface configured (in rootfs)
4. ⚠️ **Kernel**: Verify Ethernet driver is enabled (should be in default config)

### Verify Kernel Configuration

When building the kernel, ensure Ethernet driver is enabled:

```bash
cd HPS/kernel/linux-socfpga
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

Navigate to: `Device Drivers → Network device support → Ethernet driver support → STMicroelectronics devices`

Verify:
- `STMicroelectronics Multi-Gigabit Ethernet driver` is enabled
- `DWMAC Generic` is enabled
- `DWMAC_SOCFPGA` is enabled

Or check the config file:
```bash
grep -i "STMMAC\|DWMAC" build/.config
```

## Testing Ethernet

After booting the DE10-Nano:

```bash
# Check if Ethernet interface exists
ip addr show eth0

# Check if driver is loaded
dmesg | grep -i ethernet
dmesg | grep -i gmac

# Test network connectivity
ping -c 3 google.com
```

## Troubleshooting

### Ethernet Interface Not Found

1. **Check kernel driver:**
   ```bash
   lsmod | grep stmmac
   dmesg | grep -i ethernet
   ```

2. **Check device tree:**
   ```bash
   cat /proc/device-tree/sopc@0/ethernet@ff702000/status
   # Should show: "okay"
   ```

3. **Verify hardware:**
   - Check Ethernet cable is connected
   - Check link status: `ethtool eth0`

### No Network Connectivity

1. **Check interface configuration:**
   ```bash
   ip addr show eth0
   ```

2. **Request DHCP address:**
   ```bash
   dhclient eth0
   ```

3. **Check routing:**
   ```bash
   ip route show
   ```

4. **Test connectivity:**
   ```bash
   ping -c 3 8.8.8.8  # Test DNS server
   ping -c 3 google.com  # Test DNS resolution
   ```

## Summary

**For Internet Access:**
- ✅ **HPS Ethernet**: Already configured in hardware and device tree
- ❌ **FPGA Ethernet**: **NOT NEEDED** - Ethernet is in HPS, not FPGA
- ✅ **Software**: Network interface already configured in rootfs
- ⚠️ **Kernel**: Verify STMMAC driver is enabled (should be by default)

**You do NOT need to implement Ethernet in the FPGA.** The HPS has built-in Ethernet that is already configured and ready to use.
