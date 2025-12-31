#include <cstdio>
#include <cstdlib>

#include "Vtop.h"
#include "verilated.h"

#include "device.h"
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

  // Initialize physical memory and devices
  init_pmem(img_file);
  init_device();
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
  printf("Initial PC after reset: 0x%08x, Inst=0x%08x\n",
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
      int exit_code = get_ebreak_code();
      if (exit_code == 0) {
        printf("\n[HIT GOOD TRAP] ebreak executed at cycle %lu, exit code = "
               "%d\n",
               cycle_count, exit_code);
        success = true;
      } else {
        printf("\n[HIT BAD TRAP] ebreak executed at cycle %lu, exit code = "
               "%d\n",
               cycle_count, exit_code);
        success = false;
      }
      break;
    }

    // Check for invalid instruction (all zeros = illegal instruction)
    // Skip check for first few cycles during initialization
    if (cycle_count > 5) {
      uint32_t current_inst = top->instruction_debug;
      if (current_inst == 0x00000000) {
        printf("\n[HIT BAD TRAP] Invalid instruction (0x00000000) detected at "
               "PC=0x%08x, cycle %lu\n",
               top->instruction_addr_debug, cycle_count);
        break;
      }
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
