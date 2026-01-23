# ============================================================================
# HPS Application Build System for DE10-Nano
# ============================================================================
# Builds user-space applications (calculator_test, led_examples)
# ============================================================================

SHELL := /bin/bash

# Cross-compilation toolchain
CROSS_COMPILE ?= arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip

# Paths
APPLICATIONS_DIR := $(CURDIR)
HPS_DIR := $(abspath $(CURDIR)/..)
REPO_ROOT := $(abspath $(HPS_DIR)/..)

# Colors
GREEN := \033[0;32m
YELLOW := \033[1;33m
NC := \033[0m

.PHONY: all help clean calculator_test led_examples boot_led

all: calculator_test boot_led
	@echo -e "$(GREEN)===========================================$(NC)"
	@echo -e "$(GREEN)Applications build complete$(NC)"
	@echo -e "$(GREEN)===========================================$(NC)"

help:
	@echo "HPS Application Build System"
	@echo "============================"
	@echo ""
	@echo "Targets:"
	@echo "  all              - Build all applications (default)"
	@echo "  calculator_test  - Build calculator test suite"
	@echo "  boot_led         - Build boot LED indicator"
	@echo "  led_examples     - Build LED control examples"
	@echo "  clean            - Remove all build artifacts"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Cross-Compilation:"
	@echo "  Toolchain: $(CROSS_COMPILE)"
	@echo "  Compiler:  $(CC)"
	@echo ""
	@echo "To use a different toolchain:"
	@echo "  make CROSS_COMPILE=arm-none-linux-gnueabihf-"
	@echo ""
	@echo "Native compilation (on DE10-Nano):"
	@echo "  make CROSS_COMPILE="

calculator_test:
	@echo -e "$(YELLOW)Building calculator test suite...$(NC)"
	@if [ -f "calculator_test/Makefile" ]; then \
		$(MAKE) -C calculator_test CROSS_COMPILE=$(CROSS_COMPILE); \
	else \
		echo "ERROR: calculator_test/Makefile not found"; \
		exit 1; \
	fi

led_examples:
	@echo -e "$(YELLOW)Building LED examples...$(NC)"
	@if [ -f "led_examples/basic/Makefile" ]; then \
		$(MAKE) -C led_examples/basic CROSS_COMPILE=$(CROSS_COMPILE) >/dev/null 2>&1 || echo -e "$(YELLOW)Note: LED basic example skipped (requires hwlib.h on target)${NC}"; \
	fi
	@if [ -f "led_examples/advanced/Makefile" ]; then \
		$(MAKE) -C led_examples/advanced CROSS_COMPILE=$(CROSS_COMPILE) >/dev/null 2>&1 || true; \
	fi

boot_led:
	@echo -e "$(YELLOW)Building boot LED indicator...$(NC)"
	@if [ -f "boot_led/Makefile" ]; then \
		$(MAKE) -C boot_led CROSS_COMPILE=$(CROSS_COMPILE); \
	else \
		echo "ERROR: boot_led/Makefile not found"; \
		exit 1; \
	fi

clean:
	@echo -e "$(YELLOW)Cleaning application build artifacts...$(NC)"
	@if [ -f "calculator_test/Makefile" ]; then \
		$(MAKE) -C calculator_test clean || true; \
	fi
	@if [ -f "boot_led/Makefile" ]; then \
		$(MAKE) -C boot_led clean || true; \
	fi
	@if [ -f "led_examples/basic/Makefile" ]; then \
		$(MAKE) -C led_examples/basic clean || true; \
	fi
	@if [ -f "led_examples/advanced/Makefile" ]; then \
		$(MAKE) -C led_examples/advanced clean || true; \
	fi
	@echo -e "$(GREEN)Clean complete$(NC)"
