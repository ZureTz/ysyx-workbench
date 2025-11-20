#include <stdio.h>

#include "obj_dir/Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;

static Vcounter *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vcounter;
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("dump.vcd");
}

void sim_exit() {
  step_and_dump_wave();
  tfp->close();
}

int main() {
  sim_init();

  // Initialize inputs
  top->clk = 0;
  top->rst = 1;
  top->en = 0;
  top->cnt_limit = 10; // Count 0-9

  // Reset sequence
  for (int i = 0; i < 5; i++) {
    top->clk = 0;
    step_and_dump_wave();
    top->clk = 1;
    step_and_dump_wave();
  }
  top->rst = 0;
  top->en = 1;

  // Run simulation for 30 cycles
  for (int i = 0; i < 30; i++) {
    top->clk = 0;
    step_and_dump_wave();
    top->clk = 1;
    step_and_dump_wave();
    // Print status
    printf("Cycle %d: Q=%d, rco=%d\n", i, top->Q, top->rco);
  }

  // Test enable low
  printf("Testing Enable Low...\n");
  top->en = 0;
  for (int i = 0; i < 5; i++) {
    top->clk = 0;
    step_and_dump_wave();
    top->clk = 1;
    step_and_dump_wave();
    printf("Cycle (en=0) %d: Q=%d, rco=%d\n", i, top->Q, top->rco);
  }

  // Test new limit
  printf("Testing New Limit 5...\n");
  top->en = 1;
  top->cnt_limit = 5; // Count 0-4
  for (int i = 0; i < 15; i++) {
    top->clk = 0;
    step_and_dump_wave();
    top->clk = 1;
    step_and_dump_wave();
    printf("Cycle (limit=5) %d: Q=%d, rco=%d\n", i, top->Q, top->rco);
  }

  sim_exit();
  delete top;
  delete contextp;
}
