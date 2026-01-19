# Ethernet Configuration for DE10-Nano: HPS vs FPGA

## Quick Answer

**For basic internet access, you only need HPS Ethernet configuration - NO FPGA configuration needed.**

The DE10-Nano has a built-in Ethernet interface on the HPS (Hard Processor System) side that is always available and does not require any FPGA logic.

## Ethernet Architecture on DE10-Nano

### HPS Ethernet (GMAC)

The DE10-Nano SoC includes **two Ethernet MAC controllers (GMAC0 and GMAC1)** that are part of the HPS hardware:

- **GMAC0**: Base address `0xFF700000`
- **GMAC1**: Base address `0xFF702000` (typically used on DE10-Nano)
- These are **hardware peripherals** in the ARM processor subsystem
- **Not FPGA logic** - they're always available regardless of FPGA configuration

### FPGA Ethernet (Optional)

The FPGA **can** have custom Ethernet IP cores, but this is:
- **Not required** for basic internet access
- Only needed for custom high-speed Ethernet implementations
- Requires FPGA logic resources

## Current Configuration Status

### ✅ Already Configured

1. **Device Tree Configuration** (`FPGA/quartus/qsys/hps_common_board_info.xml`):
   ```xml
   <alias name="ethernet0" value="/sopc@0/ethernet@0xff702000"/>
   ```
   - Ethernet alias configured for GMAC1
   - PHY mode: RGMII
   - Reset and timing parameters configured
   - **This is automatically included when device tree is generated**

2. **HDL Pin Connections** (`FPGA/hdl/DE10_NANO_SoC_GHRD.v`):
   ```verilog
   .hps_0_hps_io_hps_io_emac1_inst_TX_CLK(HPS_ENET_GTX_CLK),
   .hps_0_hps_io_hps_io_emac1_inst_TXD0(HPS_ENET_TX_DATA[0]),
   // ... etc
   ```
   - Ethernet pins are connected to HPS I/O
   - **This is just routing - no FPGA logic needed**

3. **Linux Network Configuration** (`HPS/rootfs/configs/network/interfaces`):
   ```bash
   auto eth0
   iface eth0 inet dhcp
   ```
   - Network interface pre-configured in rootfs
   - DHCP by default (can be changed to static IP)

### ✅ What Happens Automatically

1. **Device Tree Generation**: When you run `make dtb` in FPGA directory, the device tree includes Ethernet configuration from `hps_common_board_info.xml`

2. **Kernel Support**: The Linux kernel for socfpga includes the STMMAC Ethernet driver, which automatically detects and initializes the GMAC controllers

3. **Network Interface**: Linux creates `eth0` interface automatically when the driver loads

4. **Network Configuration**: The rootfs build script configures `/etc/network/interfaces` with DHCP or static IP

## What You Need to Do

### Nothing! It's Already Configured

The Ethernet interface is **automatically configured** when you:

1. Build the device tree: `cd FPGA && make dtb`
2. Build the kernel: `cd HPS/kernel && make`
3. Build the rootfs: `cd HPS/rootfs && sudo make`
4. Create the image: `./Scripts/build_linux_image.sh`

### After Boot

The Ethernet interface should work automatically:

```bash
# Check interface
ip addr show eth0

# If not configured, run DHCP client
sudo dhclient eth0

# Test connectivity
ping google.com
```

## When Would You Need FPGA Ethernet?

You would only need FPGA Ethernet if:

1. **Custom Ethernet Protocol**: Implementing a non-standard Ethernet protocol
2. **High-Speed Processing**: Processing Ethernet packets in FPGA fabric before reaching HPS
3. **Multiple Ethernet Interfaces**: Adding additional Ethernet ports beyond the two GMAC controllers
4. **Specialized Hardware**: Custom Ethernet MAC implementation

**For standard internet access and SSH, the HPS Ethernet is sufficient and recommended.**

## Verification

### Check Device Tree

After building device tree, verify Ethernet is included:

```bash
# Check generated device tree
cat FPGA/generated/soc_system.dts | grep -i ethernet
```

You should see entries like:
```dts
ethernet@ff702000 {
    compatible = "altr,socfpga-stmmac";
    ...
};
```

### Check Kernel Support

The kernel should automatically load the Ethernet driver:

```bash
# On running board
dmesg | grep -i ethernet
# Should show: stmmac or gmac driver loaded

# Check interface
ip link show
# Should show: eth0 interface
```

### Check Network Configuration

```bash
# Check network config
cat /etc/network/interfaces

# Check interface status
ip addr show eth0
```

## Troubleshooting

### Ethernet Not Working

1. **Check device tree**: Ensure device tree includes Ethernet nodes
2. **Check kernel driver**: `dmesg | grep stmmac` or `dmesg | grep gmac`
3. **Check physical connection**: Verify Ethernet cable is connected
4. **Check PHY**: Some boards need specific PHY configuration
5. **Check network config**: Verify `/etc/network/interfaces` is correct

### Interface Not Appearing

```bash
# Force interface up
sudo ip link set eth0 up

# Request DHCP
sudo dhclient eth0

# Or configure static IP
sudo ip addr add 192.168.1.100/24 dev eth0
sudo ip route add default via 192.168.1.1
```

## Summary

| Component | Status | Required? |
|-----------|--------|-----------|
| HPS Ethernet (GMAC) | ✅ Configured in device tree | ✅ Yes - for internet access |
| FPGA Ethernet | ❌ Not configured | ❌ No - only for custom implementations |
| Linux network config | ✅ Pre-configured in rootfs | ✅ Yes - for network setup |
| Device tree | ✅ Auto-generated with Ethernet | ✅ Yes - for driver detection |

**Bottom Line**: The HPS Ethernet is already fully configured. You don't need any FPGA Ethernet implementation for basic internet access. Just build the image and boot - Ethernet should work automatically!
