#include "obj_dir/Valu_s.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;

static Valu_s *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Valu_s;
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

  // 1. Addition: 2 + 3 = 5
  top->ALUctr = 0b000;
  top->A = 0b0010;
  top->B = 0b0011;
  step_and_dump_wave();

  // 2. Subtraction: 5 - 3 = 2
  top->ALUctr = 0b001;
  top->A = 0b0101;
  top->B = 0b0011;
  step_and_dump_wave();

  // 3. Not: ~0011 = 1100
  top->ALUctr = 0b010;
  top->A = 0b0011;
  step_and_dump_wave();

  // 4. And: 0011 & 0101 = 0001
  top->ALUctr = 0b011;
  top->A = 0b0011;
  top->B = 0b0101;
  step_and_dump_wave();

  // 5. Or: 0011 | 0101 = 0111
  top->ALUctr = 0b100;
  top->A = 0b0011;
  top->B = 0b0101;
  step_and_dump_wave();

  // 6. Xor: 0011 ^ 0101 = 0110
  top->ALUctr = 0b101;
  top->A = 0b0011;
  top->B = 0b0101;
  step_and_dump_wave();

  // 7. Compare: 2 > 1 -> 1
  top->ALUctr = 0b110;
  top->A = 0b0010;
  top->B = 0b0001;
  step_and_dump_wave();

  // 8. Compare: 1 > 2 -> 0
  top->ALUctr = 0b110;
  top->A = 0b0001;
  top->B = 0b0010;
  step_and_dump_wave();

  // 9. Compare: -8 > 1 -> 0
  top->ALUctr = 0b110;
  top->A = 0b1000;
  top->B = 0b0001;
  step_and_dump_wave();

  // 10. Equal: 2 == 2 -> 1
  top->ALUctr = 0b111;
  top->A = 0b0010;
  top->B = 0b0010;
  step_and_dump_wave();

  // 11. Equal: 2 == 3 -> 0
  top->ALUctr = 0b111;
  top->A = 0b0010;
  top->B = 0b0011;
  step_and_dump_wave();

  sim_exit();
  delete top;
  delete contextp;
}
