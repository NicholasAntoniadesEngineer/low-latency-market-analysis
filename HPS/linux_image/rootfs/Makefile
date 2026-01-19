# ============================================================================
# Root Filesystem Build Makefile
# ============================================================================
# Automated rootfs build with configuration management
# ============================================================================

SHELL := /bin/bash

# Configuration
ROOTFS_DIR := $(CURDIR)/build/rootfs
ROOTFS_TAR := $(CURDIR)/build/rootfs.tar.gz
BUILD_SCRIPT := $(CURDIR)/build_rootfs.sh

# Colors
GREEN := \033[0;32m
YELLOW := \033[1;33m
NC := \033[0m

.PHONY: all rootfs clean help

all: rootfs
	@echo -e "$(GREEN)===========================================$(NC)"
	@echo -e "$(GREEN)Rootfs build complete$(NC)"
	@echo -e "$(GREEN)===========================================$(NC)"
	@echo -e "$(GREEN)Rootfs tarball: $(ROOTFS_TAR)$(NC)"

help:
	@echo "Root Filesystem Build System"
	@echo "============================"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build rootfs (default)"
	@echo "  rootfs  - Build rootfs tarball"
	@echo "  clean   - Clean build artifacts"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  Set environment variables before running:"
	@echo "    NETWORK_MODE=dhcp|static"
	@echo "    STATIC_IP=192.168.1.100"
	@echo "    STATIC_GATEWAY=192.168.1.1"
	@echo "    SSH_ENABLED=yes|no"
	@echo "    ROOT_PASSWORD=root"
	@echo ""
	@echo "Example:"
	@echo "  make NETWORK_MODE=static STATIC_IP=192.168.1.100 rootfs"

rootfs: $(ROOTFS_TAR)
	@echo -e "$(GREEN)Rootfs built successfully$(NC)"

$(ROOTFS_TAR): $(BUILD_SCRIPT)
	@echo -e "$(YELLOW)Building rootfs...$(NC)"
	@echo -e "$(YELLOW)This requires root access and may take 10-20 minutes...$(NC)"
	@if [ "$(shell id -u)" -ne 0 ]; then \
		echo -e "$(YELLOW)Note: This requires root access. Run with: sudo make rootfs$(NC)"; \
		sudo bash $(BUILD_SCRIPT); \
	else \
		bash $(BUILD_SCRIPT); \
	fi

clean:
	@echo -e "$(YELLOW)Cleaning rootfs build...$(NC)"
	@rm -rf $(CURDIR)/build
	@echo -e "$(GREEN)Clean complete$(NC)"
