#pragma once

// Physical memory size (128MB by default)
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_BASE 0x80000000

// Initialize physical memory
void init_pmem(const char *img_file);

// DPI-C functions for memory access
extern "C" int pmem_read(int raddr);
extern "C" void pmem_write(int waddr, int wdata, char wmask);
