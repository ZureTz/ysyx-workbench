#include "obj_dir/Vadder.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;

static Vadder *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vadder;
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

  // 1. Addition: 0 + 0 = 0
  top->addsub = 0;
  top->A = 0b0000;
  top->B = 0b0000;
  step_and_dump_wave();

  // 2. Addition: 2 + 3 = 5
  top->addsub = 0;
  top->A = 0b0010;
  top->B = 0b0011;
  step_and_dump_wave();

  // 3. Addition: 7 + 1 = -8 (Overflow)
  // 0111 + 0001 = 1000
  top->addsub = 0;
  top->A = 0b0111;
  top->B = 0b0001;
  step_and_dump_wave();

  // 4. Addition: -1 + 1 = 0 (Carry)
  // 1111 + 0001 = 10000
  top->addsub = 0;
  top->A = 0b1111;
  top->B = 0b0001;
  step_and_dump_wave();

  // 5. Subtraction: 5 - 3 = 2
  // 0101 - 0011
  top->addsub = 1;
  top->A = 0b0101;
  top->B = 0b0011;
  step_and_dump_wave();

  // 6. Subtraction: 3 - 5 = -2 (Borrow)
  // 0011 - 0101
  top->addsub = 1;
  top->A = 0b0011;
  top->B = 0b0101;
  step_and_dump_wave();

  // 7. Subtraction: -8 - 1 = 7 (Overflow)
  // 1000 - 0001
  top->addsub = 1;
  top->A = 0b1000;
  top->B = 0b0001;
  step_and_dump_wave();

  // 8. Subtraction: -1 - (-1) = 0
  // 1111 - 1111
  top->addsub = 1;
  top->A = 0b1111;
  top->B = 0b1111;
  step_and_dump_wave();

  sim_exit();
  delete top;
  delete contextp;
}
