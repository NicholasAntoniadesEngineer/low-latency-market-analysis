# ============================================================================
# Linux Image Build System for DE10-Nano
# ============================================================================
# Builds kernel, rootfs, and creates SD card image
# ============================================================================

SHELL := /bin/bash

# Paths
LINUX_IMAGE_DIR := $(CURDIR)
HPS_DIR := $(abspath $(CURDIR)/..)
REPO_ROOT := $(abspath $(HPS_DIR)/..)
FPGA_DIR := $(REPO_ROOT)/FPGA

# Configuration
KERNEL_DIR := $(LINUX_IMAGE_DIR)/kernel
ROOTFS_DIR := $(LINUX_IMAGE_DIR)/rootfs
SCRIPTS_DIR := $(LINUX_IMAGE_DIR)/scripts
BUILD_DIR := $(LINUX_IMAGE_DIR)/build

# Cross-compilation
CROSS_COMPILE ?= arm-linux-gnueabihf-
ARCH := arm

# Colors
GREEN := \033[0;32m
YELLOW := \033[1;33m
NC := \033[0m

.PHONY: all help kernel rootfs linux-image sd-image clean

all: linux-image
	@echo -e "$(GREEN)===========================================$(NC)"
	@echo -e "$(GREEN)Linux image build complete$(NC)"
	@echo -e "$(GREEN)===========================================$(NC)"

help:
	@echo "Linux Image Build System for DE10-Nano"
	@echo "======================================"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build complete Linux image (kernel + rootfs + SD image)"
	@echo "  kernel        - Build Linux kernel only"
	@echo "  rootfs        - Build root filesystem only (requires root)"
	@echo "  linux-image   - Build complete Linux image (kernel + rootfs + SD image)"
	@echo "  sd-image      - Create SD card image only (requires kernel and rootfs)"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make kernel        # Build kernel only"
	@echo "  sudo make rootfs   # Build rootfs only"
	@echo "  sudo make all      # Build complete image"
	@echo ""
	@echo "Deployment:"
	@echo "  cd scripts"
	@echo "  sudo ./deploy_image.sh /dev/sdX"

kernel:
	@echo -e "$(YELLOW)Building Linux kernel...$(NC)"
	@if [ -f "$(KERNEL_DIR)/Makefile" ]; then \
		$(MAKE) -C $(KERNEL_DIR) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH); \
	else \
		echo "ERROR: Kernel Makefile not found: $(KERNEL_DIR)/Makefile"; \
		exit 1; \
	fi

rootfs:
	@echo -e "$(YELLOW)Building root filesystem...$(NC)"
	@echo -e "$(YELLOW)Note: Rootfs build requires root access$(NC)"
	@if [ -f "$(ROOTFS_DIR)/Makefile" ]; then \
		if [ "$(shell id -u)" -eq 0 ]; then \
			$(MAKE) -C $(ROOTFS_DIR); \
		else \
			echo "Run with: sudo make rootfs"; \
			sudo $(MAKE) -C $(ROOTFS_DIR); \
		fi \
	else \
		echo "ERROR: Rootfs Makefile not found: $(ROOTFS_DIR)/Makefile"; \
		exit 1; \
	fi

linux-image: kernel rootfs sd-image
	@echo -e "$(GREEN)Complete Linux image built$(NC)"

sd-image: kernel rootfs
	@echo -e "$(YELLOW)Creating SD card image...$(NC)"
	@if [ -f "$(SCRIPTS_DIR)/create_sd_image.sh" ]; then \
		if [ "$(shell id -u)" -eq 0 ]; then \
			bash $(SCRIPTS_DIR)/create_sd_image.sh; \
		else \
			echo "Image creation requires root access"; \
			echo "Run with: sudo make sd-image"; \
			sudo bash $(SCRIPTS_DIR)/create_sd_image.sh; \
		fi \
	else \
		echo "ERROR: SD image script not found: $(SCRIPTS_DIR)/create_sd_image.sh"; \
		exit 1; \
	fi

clean:
	@echo -e "$(YELLOW)Cleaning Linux image build artifacts...$(NC)"
	@if [ -f "$(KERNEL_DIR)/Makefile" ]; then \
		$(MAKE) -C $(KERNEL_DIR) kernel-clean || true; \
	fi
	@if [ -f "$(ROOTFS_DIR)/Makefile" ]; then \
		$(MAKE) -C $(ROOTFS_DIR) clean || true; \
	fi
	@rm -rf $(BUILD_DIR)
	@echo -e "$(GREEN)Clean complete$(NC)"
