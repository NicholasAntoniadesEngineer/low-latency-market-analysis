# ============================================================================
# Linux Driver Build System for DE10-Nano
# ============================================================================
# Builds and integrates Linux drivers for FPGA peripherals
# ============================================================================

SHELL := /bin/bash

# Paths
DRIVERS_DIR := $(CURDIR)
HPS_DIR := $(abspath $(CURDIR)/..)
REPO_ROOT := $(abspath $(HPS_DIR)/..)

# Cross-compilation
CROSS_COMPILE ?= arm-linux-gnueabihf-

# Colors
GREEN := \033[0;32m
YELLOW := \033[1;33m
NC := \033[0m

.PHONY: all help clean integration

all: help

help:
	@echo "Linux Driver Build System for DE10-Nano"
	@echo "========================================"
	@echo ""
	@echo "This directory contains driver integration tools."
	@echo ""
	@echo "Integration Scripts:"
	@echo "  integration/integrate_linux_driver.sh  - Integrate calculator driver into kernel"
	@echo "  integration/test_integration.sh         - Test driver integration"
	@echo ""
	@echo "Usage:"
	@echo "  cd integration"
	@echo "  ./integrate_linux_driver.sh -k /path/to/linux-kernel"
	@echo ""
	@echo "Note: Driver integration is typically done during kernel build"
	@echo "      See: HPS/linux_image/kernel/Makefile"

integration:
	@echo -e "$(YELLOW)Driver integration tools are in integration/ directory$(NC)"
	@echo "See integration/README.md for usage"

clean:
	@echo -e "$(YELLOW)Cleaning driver build artifacts...$(NC)"
	@rm -rf build
	@echo -e "$(GREEN)Clean complete$(NC)"
