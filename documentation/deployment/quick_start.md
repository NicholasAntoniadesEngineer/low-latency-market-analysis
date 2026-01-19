# DE10-Nano Quick Start Guide

Minimal steps to get your FPGA bitstream and HPS software running on the DE10-Nano board.

## Prerequisites

- DE10-Nano board with prebuilt Linux image on SD card
- Development machine with Quartus Prime and ARM cross-compiler
- Ethernet connection to board (for network deployment)
- Board IP address (find via router DHCP table or serial console)

## Quick Start (5 Steps)

### Step 1: Build FPGA Bitstream

```bash
cd FPGA
make qsys-generate  # Generate QSys system
make sof            # Compile FPGA design
make rbf            # Convert to RBF format
```

**Output:** `FPGA/build/output_files/DE10_NANO_SoC_GHRD.rbf`

### Step 2: Build HPS Software

```bash
cd HPS
make
```

**Output:** `HPS/calculator_test/calculator_test`

### Step 3: Deploy to Board

**Option A: Automated Script (Recommended)**

```bash
# From repository root
./Scripts/deploy_to_board.sh -i <board-ip>
```

**Option B: Manual Transfer**

```bash
# Transfer RBF file
scp FPGA/build/output_files/DE10_NANO_SoC_GHRD.rbf root@<board-ip>:/root/soc_system.rbf

# Transfer test executable
scp HPS/calculator_test/calculator_test root@<board-ip>:/root/

# Load FPGA bitstream
ssh root@<board-ip> "echo soc_system.rbf > /sys/class/fpga_manager/fpga0/firmware"
```

### Step 4: Verify FPGA Configuration

```bash
ssh root@<board-ip> "cat /sys/class/fpga_manager/fpga0/state"
# Should show: "operating"
```

### Step 5: Run Tests

**Option A: Remote Execution**

```bash
./Scripts/remote_test.sh -i <board-ip>
```

**Option B: SSH and Run**

```bash
ssh root@<board-ip>
sudo ./calculator_test
```

## Expected Output

```
========================================================================
                   FPGA CALCULATOR TEST SUITE
========================================================================
Hardware-Accelerated Floating Point Calculator Verification
DE10-Nano SoC - HPS to FPGA Communication Test
========================================================================

Initializing calculator driver...
Calculator driver initialized
  Physical base: 0xFF280000
  Virtual base:  0xb6f80000

✓ Calculator driver initialized successfully

Running 30 test cases...

[Test 1/30] Basic addition: 1.0 + 2.0 = 3.0
  Status:       ✓ PASS

[... more tests ...]

========================================================================
                        TEST SUMMARY
========================================================================
Total tests:    30
Passed:         30
Failed:         0
Success rate:   100.0%
========================================================================
✓ ALL TESTS PASSED!
```

## Troubleshooting

**FPGA not configured:**
```bash
# Check state
ssh root@<board-ip> "cat /sys/class/fpga_manager/fpga0/state"

# Reload if needed
ssh root@<board-ip> "echo soc_system.rbf > /sys/class/fpga_manager/fpga0/firmware"
```

**Cannot connect to board:**
- Check Ethernet cable
- Verify board is powered on
- Find IP: Check router DHCP table or use `nmap -sn 192.168.1.0/24`
- Use serial console as fallback

**Tests fail:**
- Verify FPGA is configured (see above)
- Check running as root: `sudo ./calculator_test`
- Review verbose output: `sudo ./calculator_test -v`

## Next Steps

- Read [Deployment Workflow Guide](deploymentWorkflow.md) for detailed information
- Explore [HPS Software Documentation](../../HPS/README.md)
- Check [FPGA Build Documentation](../../FPGA/README.md)

---

**Last Updated:** 2026-01-17
