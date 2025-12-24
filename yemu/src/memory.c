#include "memory.h"

uint8_t M[NMEM] = {
    // 内存, 其中包含一个计算z = x + y的程序
    0b11100110, // load  6#     | R[0] <- M[y]
    0b00000100, // mov   r1, r0 | R[1] <- R[0]
    0b11100101, // load  5#     | R[0] <- M[x]
    0b00010001, // add   r0, r1 | R[0] <- R[0] + R[1]
    0b11110111, // store 7#     | M[z] <- R[0]
    0b00010000, // x = 16
    0b00100001, // y = 33
    0b00000000, // z = 0
};

void memory_init() {
  // Memory initialization if needed
  // Currently using static initialization above
}

uint8_t memory_read(uint8_t addr) {
  if (addr >= NMEM) {
    printf("Memory read out of bounds: addr = %d\n", addr);
    return 0;
  }
  return M[addr];
}

void memory_write(uint8_t addr, uint8_t data) {
  if (addr >= NMEM) {
    printf("Memory write out of bounds: addr = %d\n", addr);
    return;
  }
  M[addr] = data;
}
