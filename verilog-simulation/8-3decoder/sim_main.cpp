#include "obj_dir/Vdecoder83.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;

static Vdecoder83 *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vdecoder83;
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

  // Test Case 1: Enable = 0
  top->en = 0;
  top->X = 0b11111111;
  step_and_dump_wave();
  
  top->X = 0b00000000;
  step_and_dump_wave();

  // Test Case 2: Enable = 1, X = 0
  top->en = 1;
  top->X = 0;
  step_and_dump_wave();

  // Test Case 3: Priority Encoding
  // X[7] = 1 -> 7
  top->X = 0b10000000;
  step_and_dump_wave();

  // X[6] = 1, X[7] = 0 -> 6
  top->X = 0b01000000;
  step_and_dump_wave();

  // X[5] = 1, higher 0 -> 5
  top->X = 0b00100000;
  step_and_dump_wave();

  // X[4] = 1 -> 4
  top->X = 0b00010000;
  step_and_dump_wave();

  // X[3] = 1 -> 3
  top->X = 0b00001000;
  step_and_dump_wave();

  // X[2] = 1 -> 2
  top->X = 0b00000100;
  step_and_dump_wave();

  // X[1] = 1 -> 1
  top->X = 0b00000010;
  step_and_dump_wave();

  // X[0] = 1 -> 0
  top->X = 0b00000001;
  step_and_dump_wave();

  // Mixed inputs to test priority
  // 00001110 -> Should be 3 (X[3] is highest)
  top->X = 0b00001110;
  step_and_dump_wave();

  // 10101010 -> Should be 7
  top->X = 0b10101010;
  step_and_dump_wave();

  sim_exit();
  delete top;
  delete contextp;
}
