#include "obj_dir/Vshiftreg.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;
static Vshiftreg *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vshiftreg;
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

  top->clk = 0;
  top->din = 0;
  top->func = 0;
  top->lin = 0;
  step_and_dump_wave();

  // 1. Test Clear (000)
  printf("Test Clear (000)\n");
  top->func = 0b000;
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected 00)\n", top->dout);
  assert(top->dout == 0x00);

  // 2. Test Load (001)
  printf("Test Load (001)\n");
  top->func = 0b001;
  top->din = 0xAA; // 10101010
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected AA)\n", top->dout);
  assert(top->dout == 0xAA);

  // 3. Test Logical Right Shift (010)
  printf("Test Logical Right Shift (010)\n");
  top->func = 0b010;
  // Current dout is 0xAA (10101010) -> 01010101 (0x55)
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected 55)\n", top->dout);
  assert(top->dout == 0x55);

  // 4. Test Logical Left Shift (011)
  printf("Test Logical Left Shift (011)\n");
  top->func = 0b011;
  // Current dout is 0x55 (01010101) -> 10101010 (0xAA)
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected AA)\n", top->dout);
  assert(top->dout == 0xAA);

  // 5. Test Arithmetic Right Shift (100)
  printf("Test Arithmetic Right Shift (100)\n");
  // Load 0xF0 (11110000) first to test sign extension
  top->func = 0b001;
  top->din = 0xF0;
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();

  top->func = 0b100;
  // 11110000 -> 11111000 (0xF8)
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected F8)\n", top->dout);
  assert(top->dout == 0xF8);

  // 6. Test Serial In Left (101)
  printf("Test Serial In Left (101)\n");
  // Clear first
  top->func = 0b000;
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();

  top->func = 0b101;
  // Input sequence: 1, 0, 1, 1, 0, 1, 0, 0
  int inputs[] = {1, 0, 1, 1, 0, 1, 0, 0};
  for (int i = 0; i < 8; i++) {
    top->lin = inputs[i];
    top->clk = 0;
    step_and_dump_wave();
    top->clk = 1;
    step_and_dump_wave();
  }
  // Expected: 00101101 (0x2D)
  printf("  dout: %02x (Expected 2D)\n", top->dout);
  assert(top->dout == 0x2D);

  // 7. Test Rotate Right (110)
  printf("Test Rotate Right (110)\n");
  // Load 0x03 (00000011)
  top->func = 0b001;
  top->din = 0x03;
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();

  top->func = 0b110;
  // 00000011 -> 10000001 (0x81)
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected 81)\n", top->dout);
  assert(top->dout == 0x81);

  // 8. Test Rotate Left (111)
  printf("Test Rotate Left (111)\n");
  // Load 0xC0 (11000000)
  top->func = 0b001;
  top->din = 0xC0;
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();

  top->func = 0b111;
  // 11000000 -> 10000001 (0x81)
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
  printf("  dout: %02x (Expected 81)\n", top->dout);
  assert(top->dout == 0x81);

  sim_exit();
  delete top;
  delete contextp;
  return 0;
}
