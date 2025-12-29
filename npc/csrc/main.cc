#include <memory>
#include <print>
#include <vector>

#include "Vtop.h"
#include "verilated.h"

#include "parser.h"

constexpr int MAX_SIMULATION_CYCLES = 1000;

// Instruction memory
// 0: 10001011 (8b) -> li r0, 11  (limit = 11)
// 1: 10010000 (90) -> li r1, 0   (sum = 0)
// 2: 10100001 (a1) -> li r2, 1   (increment = 1)
// 3: 10110000 (b0) -> li r3, 0   (current number i = 0)
// 4: 00010111 (17) -> add r1, r1, r3 (sum = sum + i)
// 5: 00111110 (3e) -> add r3, r3, r2 (i = i + 1)
// 6: 11010011 (d3) -> bner0 4, r3    (if i != limit, goto 4)
std::vector<uint8_t> rom = {
    0x8b, 0x90, 0xa1, 0xb0, 0x17, 0x3e, 0xd3, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void step(Vtop *top) {
  top->clk = 0;
  top->eval();
  top->clk = 1;
  top->eval();
  top->clk = 0;
  top->eval();
}

int main(int argc, const char *argv[]) {
  Verilated::commandArgs(argc, argv);
  const auto top = std::make_shared<Vtop>();

  // Load instructions
  for (int i = 0; i < 16; i++) {
    top->instructions[i] = rom[i];
  }

  // Reset
  top->rst = 1;
  step(top.get());
  top->rst = 0;

  std::println("Simulation start");

  // Simulation loop
  for (int i = 0; i < MAX_SIMULATION_CYCLES; i++) {
    std::println(
        "Cycle {}: PC={}, Inst={:02x}, r0={}, r1={}, r2={}, r3={}, asm={}", i,
        top->pc, rom[top->pc], top->regs[0], top->regs[1], top->regs[2],
        top->regs[3], parse_instruction(rom[top->pc]));

    step(top.get());

    // Stop if PC points to beyond instruction memory
    if (top->pc >= 16) {
      break;
    }

    // Check for result
    if (top->pc == 7 && top->regs[1] == 55) {
      std::println("Result verified: sum = {}", top->regs[1]);
      break;
    }
  }

  if (top->regs[1] == 55) {
    std::println("Test PASSED.");
  } else {
    std::println("Test FAILED. Expected 55, got {}", top->regs[1]);
  }

  std::println("Simulation end");
  return 0;
}