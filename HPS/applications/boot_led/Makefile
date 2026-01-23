# ============================================================================
# Boot LED Indicator - Makefile
# ============================================================================
# Cross-compilation Makefile for ARM (HPS on DE10-Nano)
# ============================================================================

TARGET = boot_led

# Cross-compilation toolchain
CROSS_COMPILE ?= arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip

# Compiler flags
CFLAGS = -Wall -Wextra -O2
CFLAGS += -std=gnu99
CFLAGS += -D_GNU_SOURCE

# LED PIO offset can be overridden
# Common values: 0x0 (GHRD default), 0x10010 (some designs)
ifdef LED_OFFSET
CFLAGS += -DLED_PIO_OFFSET=$(LED_OFFSET)
endif

# Source files
SRCS = boot_led.c
OBJS = $(SRCS:.c=.o)

# ============================================================================
# Build Rules
# ============================================================================

.PHONY: all clean install help

all: $(TARGET)
	@echo "Build complete: $(TARGET)"
	@echo ""
	@echo "To deploy to DE10-Nano:"
	@echo "  scp $(TARGET) root@<board-ip>:/usr/local/bin/"

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -f $(TARGET) $(OBJS) *~
	@echo "Clean complete"

strip: $(TARGET)
	@echo "Stripping debug symbols..."
	$(STRIP) $(TARGET)
	@ls -lh $(TARGET)

help:
	@echo "Boot LED Indicator - Makefile Help"
	@echo "==================================="
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build boot_led (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  strip    - Strip debug symbols"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  CROSS_COMPILE - Toolchain prefix (default: arm-linux-gnueabihf-)"
	@echo "  LED_OFFSET    - LED PIO offset from bridge base (default: 0x0)"
	@echo ""
	@echo "Examples:"
	@echo "  make                           # Build for ARM"
	@echo "  make CROSS_COMPILE=            # Native build (on DE10-Nano)"
	@echo "  make LED_OFFSET=0x10010        # Custom LED offset"
