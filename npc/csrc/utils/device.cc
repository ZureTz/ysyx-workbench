#include "device.h"
#include <cstdio>

// Forward declarations for device functions
extern uint32_t serial_port_read(uint32_t addr);
extern void serial_port_write(uint32_t addr, uint32_t data, uint8_t wmask);
extern uint32_t rtc_port_read(uint32_t addr);
extern void rtc_port_write(uint32_t addr, uint32_t data, uint8_t wmask);
extern void init_timer();

// Initialize all devices
void init_device() {
  init_timer();
  printf("Devices initialized: Serial Port @ 0x%08x, RTC @ 0x%08x\n",
         SERIAL_PORT_BASE, RTC_ADDR_BASE);
}

// Check if address is mapped to a device
bool is_device_addr(uint32_t addr) {
  // Align address to 4 bytes for comparison
  uint32_t aligned_addr = addr & ~0x3u;

  // Serial port
  if (aligned_addr >= SERIAL_PORT_BASE &&
      aligned_addr < SERIAL_PORT_BASE + SERIAL_PORT_SIZE) {
    return true;
  }

  // RTC
  if (aligned_addr >= RTC_ADDR_BASE &&
      aligned_addr < RTC_ADDR_BASE + RTC_ADDR_SIZE) {
    return true;
  }

  return false;
}

// Read from device
uint32_t device_read(uint32_t addr) {
  uint32_t aligned_addr = addr & ~0x3u;

  // Serial port
  if (aligned_addr >= SERIAL_PORT_BASE &&
      aligned_addr < SERIAL_PORT_BASE + SERIAL_PORT_SIZE) {
    return serial_port_read(aligned_addr);
  }

  // RTC
  if (aligned_addr >= RTC_ADDR_BASE &&
      aligned_addr < RTC_ADDR_BASE + RTC_ADDR_SIZE) {
    return rtc_port_read(aligned_addr);
  }

  printf("Warning: Read from unmapped device address 0x%08x\n", addr);
  return 0;
}

// Write to device
void device_write(uint32_t addr, uint32_t data, uint8_t wmask, uint32_t pc) {
  uint32_t aligned_addr = addr & ~0x3u;

  // Debug output for all device writes
  // fprintf(stderr, "[DEVICE] PC=0x%08x write to 0x%08x, data=0x%08x,
  // wmask=0x%x\n",
  //         pc, addr, data, wmask);

  // Serial port
  if (aligned_addr >= SERIAL_PORT_BASE &&
      aligned_addr < SERIAL_PORT_BASE + SERIAL_PORT_SIZE) {
    serial_port_write(aligned_addr, data, wmask);
    return;
  }

  // RTC
  if (aligned_addr >= RTC_ADDR_BASE &&
      aligned_addr < RTC_ADDR_BASE + RTC_ADDR_SIZE) {
    rtc_port_write(aligned_addr, data, wmask);
    return;
  }

  printf("Warning: Write to unmapped device address 0x%08x\n", addr);
}
