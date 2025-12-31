#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "device.h"
#include "pmem.h"

// Physical memory storage
static uint8_t pmem[PMEM_SIZE] = {0};

// Convert guest physical address to host virtual address
static uint8_t *guest_to_host(uint32_t paddr) {
  return pmem + (paddr - PMEM_BASE);
}

// Check if address is in valid memory range
static bool in_pmem(uint32_t addr) {
  return (addr >= PMEM_BASE) && (addr < PMEM_BASE + PMEM_SIZE);
}

// Initialize physical memory from image file
void init_pmem(const char *img_file) {
  if (img_file == nullptr) {
    printf("No image file specified, using empty memory\n");
    return;
  }

  FILE *fp = fopen(img_file, "rb");
  if (fp == nullptr) {
    printf("Warning: Cannot open image file '%s', using empty memory\n",
           img_file);
    return;
  }

  // Get file size
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // Check if image fits in memory
  if (size > PMEM_SIZE) {
    printf("Error: Image file too large (%ld bytes, max %d bytes)\n", size,
           PMEM_SIZE);
    fclose(fp);
    exit(1);
  }

  // Read image into memory
  size_t ret = fread(pmem, 1, size, fp);
  if (ret != (size_t)size) {
    printf("Error: Failed to read image file\n");
    fclose(fp);
    exit(1);
  }

  fclose(fp);
  printf("Loaded image file '%s' (%ld bytes)\n", img_file, size);
}

// DPI-C function: Read 4 bytes from aligned address
extern "C" int pmem_read(int raddr, int pc, unsigned char is_instruction) {
  uint32_t addr = (uint32_t)raddr & ~0x3u; // Align to 4 bytes

  // Check if this is a device address (MMIO)
  if (is_device_addr(addr)) {
    // Don't allow instruction fetch from device
    if (is_instruction) {
      printf("Error: Trying to fetch instruction from device address 0x%08x "
             "(PC=0x%08x)\n",
             addr, (uint32_t)pc);
      return 0;
    }
    return device_read(addr);
  }

  if (!in_pmem(addr)) {
    if (is_instruction) {
      // Instruction fetch from invalid address is a critical error
      printf("Error: Read instruction from address 0x%08x out of bound "
             "(PC=0x%08x)\n",
             addr, (uint32_t)pc);
    }
    // For data access, silently return 0 (many addresses may be probed but not
    // actually used)
    return 0;
  }

  uint32_t *p = (uint32_t *)guest_to_host(addr);
  return *p;
}

// DPI-C function: Write up to 4 bytes to aligned address with byte mask
extern "C" void pmem_write(int waddr, int wdata, char wmask, int pc) {
  uint32_t addr = (uint32_t)waddr & ~0x3u; // Align to 4 bytes

  // Check if this is a device address (MMIO)
  if (is_device_addr(addr)) {
    device_write(addr, wdata, wmask, pc);
    return;
  }

  if (!in_pmem(addr)) {
    printf("Error: Write address 0x%08x out of bound (PC=0x%08x)\n", addr,
           (uint32_t)pc);
    return;
  }

  uint8_t *p = guest_to_host(addr);

  // Apply byte mask to write individual bytes
  for (int i = 0; i < 4; i++) {
    if (wmask & (1 << i)) {
      p[i] = (wdata >> (i * 8)) & 0xff;
    }
  }
}
