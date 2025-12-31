#pragma once

#include <cstdint>

// Device address mapping (与SoC保持一致)
// Serial port (UART)
#define SERIAL_PORT_BASE 0x10000000
#define SERIAL_PORT_SIZE 8

// RTC (Real-Time Clock)
#define RTC_ADDR_BASE 0xa0000048
#define RTC_ADDR_SIZE 8

// Initialize all devices
void init_device();

// Check if address is mapped to a device
bool is_device_addr(uint32_t addr);

// Read from device
uint32_t device_read(uint32_t addr);

// Write to device
void device_write(uint32_t addr, uint32_t data, uint8_t wmask, uint32_t pc);
