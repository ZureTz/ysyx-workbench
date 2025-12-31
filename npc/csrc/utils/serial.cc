#include <cstdio>
#include "device.h"

// Serial port data register
// Writing to this register outputs a character

static uint32_t serial_read(uint32_t offset) {
  // Serial status register: always ready (simplified)
  // For simplicity, we just return 0 for reads
  return 0;
}

static void serial_write(uint32_t offset, uint32_t data, uint8_t wmask) {
  // Check if writing to data register (offset 0)
  if (offset == 0) {
    // Output the character based on wmask
    // The wmask indicates which byte to write
    // Use independent if statements instead of else-if to handle multiple bytes
    if (wmask & 0x1) {
      putchar((char)(data & 0xff));
    }
    if (wmask & 0x2) {
      putchar((char)((data >> 8) & 0xff));
    }
    if (wmask & 0x4) {
      putchar((char)((data >> 16) & 0xff));
    }
    if (wmask & 0x8) {
      putchar((char)((data >> 24) & 0xff));
    }
    fflush(stdout);
  }
}

// Export functions for device manager
uint32_t serial_port_read(uint32_t addr) {
  uint32_t offset = addr - SERIAL_PORT_BASE;
  return serial_read(offset);
}

void serial_port_write(uint32_t addr, uint32_t data, uint8_t wmask) {
  uint32_t offset = addr - SERIAL_PORT_BASE;
  serial_write(offset, data, wmask);
}
