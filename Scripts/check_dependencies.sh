#!/bin/bash
# ============================================================================
# Dependency Check Script for DE10-Nano Linux Image Build
# ============================================================================
# Verifies all required tools and dependencies are installed
# ============================================================================

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
MISSING=0
WARNINGS=0

# ============================================================================
# Functions
# ============================================================================

check_command() {
    local cmd=$1
    local required=${2:-yes}
    local install_hint=$3
    
    if command -v "$cmd" &> /dev/null; then
        echo -e "${GREEN}✓${NC} $cmd"
        return 0
    else
        if [ "$required" = "yes" ]; then
            echo -e "${RED}✗${NC} $cmd (REQUIRED)"
            MISSING=$((MISSING + 1))
            if [ -n "$install_hint" ]; then
                echo -e "    ${YELLOW}Install: $install_hint${NC}"
            fi
        else
            echo -e "${YELLOW}⚠${NC} $cmd (optional)"
            WARNINGS=$((WARNINGS + 1))
            if [ -n "$install_hint" ]; then
                echo -e "    ${YELLOW}Install: $install_hint${NC}"
            fi
        fi
        return 1
    fi
}

check_cross_compiler() {
    local prefix=$1
    local gcc_cmd="${prefix}gcc"
    
    if command -v "$gcc_cmd" &> /dev/null; then
        local version=$($gcc_cmd --version | head -n1)
        echo -e "${GREEN}✓${NC} Cross-compiler: $prefix ($version)"
        return 0
    else
        echo -e "${RED}✗${NC} Cross-compiler: $prefix (REQUIRED)"
        echo -e "    ${YELLOW}Install: sudo apt-get install gcc-arm-linux-gnueabihf${NC}"
        MISSING=$((MISSING + 1))
        return 1
    fi
}

print_header() {
    echo -e "${GREEN}===========================================${NC}"
    echo -e "${GREEN}$1${NC}"
    echo -e "${GREEN}===========================================${NC}"
}

# ============================================================================
# Main
# ============================================================================

print_header "Checking Dependencies for DE10-Nano Linux Image Build"

echo ""
echo "Basic Build Tools:"
echo "------------------"
check_command "make" "yes" "sudo apt-get install build-essential"
check_command "git" "yes" "sudo apt-get install git"
check_command "bash" "yes" "sudo apt-get install bash"
check_command "tar" "yes" "sudo apt-get install tar"
check_command "gzip" "yes" "sudo apt-get install gzip"

echo ""
echo "Cross-Compilation Toolchain:"
echo "----------------------------"
check_cross_compiler "arm-linux-gnueabihf-"

echo ""
echo "Kernel Build Tools:"
echo "-------------------"
check_command "dtc" "no" "sudo apt-get install device-tree-compiler"

echo ""
echo "Rootfs Build Tools:"
echo "-------------------"
check_command "debootstrap" "no" "sudo apt-get install debootstrap"
check_command "qemu-debootstrap" "no" "sudo apt-get install qemu-user-static"
check_command "qemu-arm-static" "no" "sudo apt-get install qemu-user-static"

echo ""
echo "SD Card Image Tools:"
echo "--------------------"
check_command "parted" "no" "sudo apt-get install parted"
check_command "mkfs.vfat" "no" "sudo apt-get install dosfstools"
check_command "mkfs.ext4" "no" "sudo apt-get install e2fsprogs"
check_command "losetup" "no" "Part of util-linux (usually pre-installed)"
check_command "dd" "yes" "Part of coreutils (usually pre-installed)"

echo ""
echo "FPGA Build Tools:"
echo "-----------------"
check_command "quartus_sh" "no" "Install Intel Quartus Prime"
if [ -n "$SOCEDS_DEST_ROOT" ]; then
    echo -e "${GREEN}✓${NC} SOCEDS_DEST_ROOT is set: $SOCEDS_DEST_ROOT"
    if command -v bsp-create-settings &> /dev/null; then
        echo -e "${GREEN}✓${NC} bsp-create-settings found"
    else
        echo -e "${YELLOW}⚠${NC} bsp-create-settings not in PATH"
        echo -e "    ${YELLOW}Add to PATH: export PATH=\$PATH:\$SOCEDS_DEST_ROOT/host_tools/bin${NC}"
        WARNINGS=$((WARNINGS + 1))
    fi
else
    echo -e "${YELLOW}⚠${NC} SOCEDS_DEST_ROOT not set (optional for FPGA build)"
    echo -e "    ${YELLOW}Set: export SOCEDS_DEST_ROOT=/path/to/intelFPGA/20.1/embedded${NC}"
    WARNINGS=$((WARNINGS + 1))
fi

echo ""
echo "Network Tools (for deployment):"
echo "-------------------------------"
check_command "ssh" "no" "sudo apt-get install openssh-client"
check_command "scp" "no" "sudo apt-get install openssh-client"

echo ""
print_header "Summary"

if [ $MISSING -eq 0 ] && [ $WARNINGS -eq 0 ]; then
    echo -e "${GREEN}All dependencies satisfied!${NC}"
    exit 0
elif [ $MISSING -eq 0 ]; then
    echo -e "${YELLOW}All required dependencies satisfied.${NC}"
    echo -e "${YELLOW}Some optional tools are missing ($WARNINGS warnings).${NC}"
    echo -e "${YELLOW}You can proceed, but some features may not be available.${NC}"
    exit 0
else
    echo -e "${RED}Missing required dependencies: $MISSING${NC}"
    if [ $WARNINGS -gt 0 ]; then
        echo -e "${YELLOW}Optional tools missing: $WARNINGS${NC}"
    fi
    echo ""
    echo -e "${YELLOW}Please install missing dependencies and run this script again.${NC}"
    exit 1
fi
