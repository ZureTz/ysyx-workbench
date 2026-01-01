#include <cstdint>

#include <time.h>

#include "device.h"

// Boot time to calculate uptime
static uint64_t boot_time = 0;

// Get current time in microseconds
static uint64_t get_time_us() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

// Initialize timer
void init_timer() { boot_time = get_time_us(); }

// Get uptime in microseconds since boot
static uint64_t get_uptime_us() { return get_time_us() - boot_time; }

// RTC has two 32-bit registers:
// - RTC_ADDR_BASE + 0: lower 32 bits of uptime (microseconds)
// - RTC_ADDR_BASE + 4: upper 32 bits of uptime (microseconds)
static uint32_t rtc_read(uint32_t offset) {
  uint64_t uptime = get_uptime_us();

  if (offset == 0) {
    // Lower 32 bits
    return (uint32_t)(uptime & 0xFFFFFFFF);
  } else if (offset == 4) {
    // Upper 32 bits
    return (uint32_t)(uptime >> 32);
  }

  return 0;
}

static void rtc_write(uint32_t offset, uint32_t data, uint8_t wmask) {
  // RTC is read-only, ignore writes
}

// Export functions for device manager
uint32_t rtc_port_read(uint32_t addr) {
  uint32_t offset = addr - RTC_ADDR_BASE;
  return rtc_read(offset);
}

void rtc_port_write(uint32_t addr, uint32_t data, uint8_t wmask) {
  uint32_t offset = addr - RTC_ADDR_BASE;
  rtc_write(offset, data, wmask);
}
