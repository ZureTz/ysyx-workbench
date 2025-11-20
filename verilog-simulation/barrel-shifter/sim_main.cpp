#include "obj_dir/Vbarrel.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;
static Vbarrel *top;

void step_and_dump_wave() {
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init() {
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vbarrel;
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

  // Test Case 1: Logical Right Shift
  printf("Test 1: Logical Right Shift\n");
  top->indata = 0x80000000; // Sign bit set
  top->lr = 0;              // Right
  top->al = 0;              // Logical
  top->shamt = 1;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: 40000000)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0x40000000);

  top->shamt = 4;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: 08000000)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0x08000000);

  // Test Case 2: Arithmetic Right Shift
  printf("Test 2: Arithmetic Right Shift\n");
  top->indata = 0x80000000;
  top->lr = 0; // Right
  top->al = 1; // Arithmetic
  top->shamt = 1;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: C0000000)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0xC0000000);

  top->shamt = 4;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: F8000000)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0xF8000000);

  top->shamt = 31;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: FFFFFFFF)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0xFFFFFFFF);

  // Test Case 3: Logical Left Shift
  printf("Test 3: Logical Left Shift\n");
  top->indata = 0x00000001;
  top->lr = 1; // Left
  top->al = 0; // Logical
  top->shamt = 1;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: 00000002)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0x00000002);

  top->shamt = 4;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: 00000010)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0x00000010);

  // Test Case 4: Arithmetic Left Shift (Should be same as Logical Left)
  printf("Test 4: Arithmetic Left Shift\n");
  top->indata = 0x00000001;
  top->lr = 1; // Left
  top->al = 1; // Arithmetic
  top->shamt = 1;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d, LR: %d, AL: %d -> Out: %08x (Exp: 00000002)\n",
         top->indata, top->shamt, top->lr, top->al, top->outdata);
  assert(top->outdata == 0x00000002);

  // Test Case 5: Zero Shift
  printf("Test 5: Zero Shift\n");
  top->indata = 0x12345678;
  top->shamt = 0;
  step_and_dump_wave();
  printf("  In: %08x, Shamt: %d -> Out: %08x (Exp: 12345678)\n", top->indata,
         top->shamt, top->outdata);
  assert(top->outdata == 0x12345678);

  sim_exit();
  delete top;
  delete contextp;
  return 0;
}
