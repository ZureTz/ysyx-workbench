#include <cstdio>
#include <cstdlib>

#include "Vtop.h"
#include "verilated.h"

#include "ebreak.h"
#include "pmem.h"

// Maximum number of simulation cycles
// #define MAX_SIM_CYCLES 10000

int main(int argc, char const *argv[]) {
  // Parse command line arguments
  const char *img_file = nullptr;
  if (argc >= 2) {
    img_file = argv[1];
  }

  // Initialize Verilator
  Verilated::commandArgs(argc, (char **)argv);
  Verilated::traceEverOn(true);

  // Create top module instance
  Vtop *top = new Vtop;

  // Initialize physical memory
  init_pmem(img_file);
  reset_ebreak();

  // Reset sequence
  top->rst = 1;
  top->clk = 0;
  top->eval();

  // One clock cycle with reset active
  top->clk = 1;
  top->eval();

  top->clk = 0;
  top->eval();

  // Release reset
  top->rst = 0;
  top->eval();

  // Simulation main loop starts here
  uint64_t sim_time = 4;
  uint64_t cycle_count = 0;
  bool success = false;

  printf("Starting NPC simulation...\n");
  printf("Initial PC after reset: 0x%08x\n", top->instruction_addr_debug);
  printf("Cycle %lu: PC=0x%08x, Inst=0x%08x\n", cycle_count,
         top->instruction_addr_debug, top->instruction_debug);

#ifdef MAX_SIM_CYCLES
  while (!Verilated::gotFinish() && cycle_count < MAX_SIM_CYCLES) {
#else
  while (!Verilated::gotFinish()) {
#endif
    // Positive edge - PC updates here, but instruction from previous PC is
    // being executed
    top->clk = 1;
    top->eval();

#ifdef MAX_SIM_CYCLES
    // Print debug info for first 10 cycles and last 10 before timeout
    if (cycle_count <= 10 || cycle_count >= MAX_SIM_CYCLES - 10) {
#else
    // Print debug info for first 10 cycles only
    if (cycle_count <= 10) {
#endif
      printf("Cycle %lu: PC=0x%08x, Inst=0x%08x\n", cycle_count,
             top->instruction_addr_debug, top->instruction_debug);
    }

    // Check for ebreak after positive edge
    if (is_ebreak()) {
      printf("\n[HIT GOOD TRAP] ebreak executed at cycle %lu\n", cycle_count);
      success = true;
      break;
    }

    // Negative edge
    top->clk = 0;
    top->eval();

    cycle_count++;
  }

  // Print simulation results
  if (!success) {
#ifdef MAX_SIM_CYCLES
    if (cycle_count >= MAX_SIM_CYCLES) {
      printf("[HIT BAD TRAP] Simulation timeout after %lu cycles\n",
             cycle_count);
    } else {
      printf("[HIT BAD TRAP] Simulation ended unexpectedly\n");
    }
#else
    printf("[HIT BAD TRAP] Simulation ended unexpectedly\n");
#endif
  }

  printf("Total cycles: %lu\n", cycle_count);

  // Cleanup
  delete top;

  return success ? 0 : 1;
}
