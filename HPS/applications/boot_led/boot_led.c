// ============================================================================
// Boot LED Indicator - DE10-Nano
// ============================================================================
// Displays LED patterns on boot to indicate the custom Linux image is running
// Uses direct memory-mapped I/O to the FPGA LEDs via lightweight HPS-FPGA bridge
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>

// ============================================================================
// Hardware Constants - DE10-Nano Cyclone V SoC
// ============================================================================

// Lightweight HPS-to-FPGA bridge base address
#define HPS_LW_BRIDGE_BASE      0xFF200000
#define HPS_LW_BRIDGE_SPAN      0x00200000  // 2MB span

// LED PIO offset from bridge base (depends on QSys configuration)
// Common offsets: 0x0 for GHRD, 0x10010 for some designs
// This can be overridden at compile time with -DLED_PIO_OFFSET=0xNNNN
#ifndef LED_PIO_OFFSET
#define LED_PIO_OFFSET          0x00000000
#endif

// Number of LEDs (DE10-Nano has 8 user LEDs)
#define LED_COUNT               8

// LED active state (DE10-Nano LEDs are active-low)
#define LED_ACTIVE_LOW          1

// ============================================================================
// Pattern Timing (microseconds)
// ============================================================================
#define STARTUP_PATTERN_DELAY_US    80000   // 80ms between startup steps
#define HEARTBEAT_ON_US             100000  // 100ms on
#define HEARTBEAT_OFF_US            100000  // 100ms off  
#define HEARTBEAT_PAUSE_US          700000  // 700ms pause between beats
#define KNIGHT_RIDER_DELAY_US       60000   // 60ms for knight rider effect

// ============================================================================
// Global State
// ============================================================================
static volatile sig_atomic_t keep_running = 1;
static volatile uint32_t *led_register = NULL;
static void *mapped_base = NULL;
static int memory_fd = -1;

// ============================================================================
// Signal Handler
// ============================================================================
static void signal_handler(int signum) {
    (void)signum;
    keep_running = 0;
}

// ============================================================================
// LED Control Functions
// ============================================================================

static void led_write(uint8_t value) {
    if (led_register == NULL) {
        return;
    }
    
#if LED_ACTIVE_LOW
    *led_register = (uint32_t)(~value & 0xFF);
#else
    *led_register = (uint32_t)(value & 0xFF);
#endif
}

static void led_all_on(void) {
    led_write(0xFF);
}

static void led_all_off(void) {
    led_write(0x00);
}

// ============================================================================
// LED Patterns
// ============================================================================

// Startup pattern: LEDs fill from left to right, then flash
static void pattern_startup(void) {
    // Fill LEDs from right to left
    for (int led_index = 0; led_index < LED_COUNT && keep_running; led_index++) {
        uint8_t mask = (1 << (led_index + 1)) - 1;  // 0x01, 0x03, 0x07, 0x0F...
        led_write(mask);
        usleep(STARTUP_PATTERN_DELAY_US);
    }
    
    if (!keep_running) return;
    
    // Flash all LEDs 3 times to indicate ready
    for (int flash_count = 0; flash_count < 3 && keep_running; flash_count++) {
        led_all_off();
        usleep(STARTUP_PATTERN_DELAY_US);
        led_all_on();
        usleep(STARTUP_PATTERN_DELAY_US);
    }
    
    // Brief pause
    usleep(STARTUP_PATTERN_DELAY_US * 2);
}

// Heartbeat pattern: double-blink like a heartbeat
static void pattern_heartbeat_cycle(void) {
    // First beat
    led_write(0x18);  // Center two LEDs
    usleep(HEARTBEAT_ON_US);
    led_all_off();
    usleep(HEARTBEAT_OFF_US);
    
    // Second beat (slightly wider)
    led_write(0x3C);  // Four center LEDs
    usleep(HEARTBEAT_ON_US);
    led_all_off();
    
    // Pause between heartbeats
    usleep(HEARTBEAT_PAUSE_US);
}

// Knight Rider / Cylon pattern
static void pattern_knight_rider_cycle(void) {
    static int position = 0;
    static int direction = 1;
    
    uint8_t mask = (0x03 << position);  // Two adjacent LEDs
    led_write(mask);
    usleep(KNIGHT_RIDER_DELAY_US);
    
    position += direction;
    if (position >= LED_COUNT - 1) {
        direction = -1;
        position = LED_COUNT - 2;
    } else if (position <= 0) {
        direction = 1;
        position = 0;
    }
}

// Binary counter pattern
static void pattern_counter_cycle(void) {
    static uint8_t counter = 0;
    led_write(counter);
    usleep(200000);  // 200ms
    counter++;
}

// ============================================================================
// Hardware Initialization
// ============================================================================

static int hardware_init(void) {
    // Open /dev/mem for physical memory access
    memory_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memory_fd < 0) {
        fprintf(stderr, "boot_led: Failed to open /dev/mem: %s\n", strerror(errno));
        fprintf(stderr, "boot_led: Run as root or with CAP_SYS_RAWIO capability\n");
        return -1;
    }
    
    // Map the lightweight HPS-FPGA bridge region
    mapped_base = mmap(NULL, HPS_LW_BRIDGE_SPAN, 
                       PROT_READ | PROT_WRITE, MAP_SHARED,
                       memory_fd, HPS_LW_BRIDGE_BASE);
    
    if (mapped_base == MAP_FAILED) {
        fprintf(stderr, "boot_led: Failed to mmap: %s\n", strerror(errno));
        close(memory_fd);
        memory_fd = -1;
        return -1;
    }
    
    // Calculate LED register address
    led_register = (volatile uint32_t *)((char *)mapped_base + LED_PIO_OFFSET);
    
    return 0;
}

static void hardware_cleanup(void) {
    // Turn off all LEDs before exit
    if (led_register != NULL) {
        led_all_off();
        led_register = NULL;
    }
    
    // Unmap memory
    if (mapped_base != NULL && mapped_base != MAP_FAILED) {
        munmap(mapped_base, HPS_LW_BRIDGE_SPAN);
        mapped_base = NULL;
    }
    
    // Close file descriptor
    if (memory_fd >= 0) {
        close(memory_fd);
        memory_fd = -1;
    }
}

// ============================================================================
// Usage
// ============================================================================

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", program_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Boot LED indicator for DE10-Nano custom Linux\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d, --daemon     Run as daemon (background, no startup pattern)\n");
    fprintf(stderr, "  -o, --oneshot    Run startup pattern once and exit\n");
    fprintf(stderr, "  -p, --pattern N  Select pattern: 0=heartbeat (default), 1=knight, 2=counter\n");
    fprintf(stderr, "  -h, --help       Show this help message\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Requires root privileges for /dev/mem access.\n");
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char *argv[]) {
    bool daemon_mode = false;
    bool oneshot_mode = false;
    int pattern_type = 0;  // 0=heartbeat, 1=knight rider, 2=counter
    
    // Parse arguments
    for (int arg_index = 1; arg_index < argc; arg_index++) {
        if (strcmp(argv[arg_index], "-d") == 0 || strcmp(argv[arg_index], "--daemon") == 0) {
            daemon_mode = true;
        } else if (strcmp(argv[arg_index], "-o") == 0 || strcmp(argv[arg_index], "--oneshot") == 0) {
            oneshot_mode = true;
        } else if (strcmp(argv[arg_index], "-p") == 0 || strcmp(argv[arg_index], "--pattern") == 0) {
            if (arg_index + 1 < argc) {
                pattern_type = atoi(argv[++arg_index]);
                if (pattern_type < 0 || pattern_type > 2) {
                    fprintf(stderr, "Invalid pattern number (0-2)\n");
                    return 1;
                }
            }
        } else if (strcmp(argv[arg_index], "-h") == 0 || strcmp(argv[arg_index], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[arg_index]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Setup signal handlers
    struct sigaction signal_action;
    memset(&signal_action, 0, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGINT, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    
    // Initialize hardware
    if (hardware_init() != 0) {
        return 1;
    }
    
    // Daemon mode: fork to background
    if (daemon_mode) {
        pid_t process_id = fork();
        if (process_id < 0) {
            fprintf(stderr, "boot_led: Failed to fork: %s\n", strerror(errno));
            hardware_cleanup();
            return 1;
        }
        if (process_id > 0) {
            // Parent exits
            return 0;
        }
        // Child continues as daemon
        setsid();
    }
    
    // Run startup pattern (unless daemon mode)
    if (!daemon_mode) {
        pattern_startup();
    }
    
    // Oneshot mode: exit after startup pattern
    if (oneshot_mode) {
        led_all_off();
        hardware_cleanup();
        return 0;
    }
    
    // Main loop - run continuous pattern
    while (keep_running) {
        switch (pattern_type) {
            case 0:
                pattern_heartbeat_cycle();
                break;
            case 1:
                pattern_knight_rider_cycle();
                break;
            case 2:
                pattern_counter_cycle();
                break;
            default:
                pattern_heartbeat_cycle();
                break;
        }
    }
    
    // Cleanup
    hardware_cleanup();
    
    return 0;
}
