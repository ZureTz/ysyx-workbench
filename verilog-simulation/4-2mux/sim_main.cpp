#include "obj_dir/Vmux42.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;

static Vmux42 *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vmux42;
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

  // Set inputs X0, X1, X2, X3
  top->X0 = 0b00;
  top->X1 = 0b01;
  top->X2 = 0b10;
  top->X3 = 0b11;

  // Select X0
  top->Y = 0b00;
  step_and_dump_wave();

  // Select X1
  top->Y = 0b01;
  step_and_dump_wave();

  // Select X2
  top->Y = 0b10;
  step_and_dump_wave();

  // Select X3
  top->Y = 0b11;
  step_and_dump_wave();

  // Change inputs
  top->X0 = 0b11;
  top->X1 = 0b10;
  top->X2 = 0b01;
  top->X3 = 0b00;

  // Select X0
  top->Y = 0b00;
  step_and_dump_wave();

  // Select X1
  top->Y = 0b01;
  step_and_dump_wave();

  // Select X2
  top->Y = 0b10;
  step_and_dump_wave();

  // Select X3
  top->Y = 0b11;
  step_and_dump_wave();

  sim_exit();
  delete top;
  delete contextp;
}
