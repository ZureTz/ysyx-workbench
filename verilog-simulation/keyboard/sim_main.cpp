#include "obj_dir/Vkeyboard.h"
#include "obj_dir/Vkeyboard___024root.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>

VerilatedContext *contextp = NULL;
VerilatedVcdC *tfp = NULL;
Vkeyboard *top = NULL;

void step(int n) {
  while (n--) {
    top->clk = 0;
    top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());
    top->clk = 1;
    top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());
  }
}

void send_ps2(uint8_t data) {
  int parity = 1; // Odd parity
  uint8_t d = data;
  for (int i = 0; i < 8; i++) {
    if (d & 1)
      parity = !parity;
    d >>= 1;
  }

  int bits[11];
  bits[0] = 0; // Start
  for (int i = 0; i < 8; i++)
    bits[i + 1] = (data >> i) & 1;
  bits[9] = parity;
  bits[10] = 1; // Stop

  for (int i = 0; i < 11; i++) {
    top->ps2_data = bits[i];
    step(10);         // Setup
    top->ps2_clk = 0; // Falling edge
    step(20);         // Hold low
    top->ps2_clk = 1; // Rising edge
    step(10);         // Hold high
  }
  step(100);
}

int main(int argc, char **argv) {
  contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  contextp->traceEverOn(true);
  top = new Vkeyboard{contextp};
  tfp = new VerilatedVcdC;
  top->trace(tfp, 99);
  tfp->open("dump.vcd");

  // Initialize RAM (Scan code to ASCII) and Map (ASCII to Scan code)
  std::map<char, uint8_t> ascii_to_scan;

  auto add_mapping = [&](char c, uint8_t sc) {
    ascii_to_scan[c] = sc;
    top->rootp->keyboard__DOT__myram__DOT__ascii_tab[sc] = c;
  };

  add_mapping('a', 0x1C);
  add_mapping('b', 0x32);
  add_mapping('c', 0x21);
  add_mapping('d', 0x23);
  add_mapping('e', 0x24);
  add_mapping('f', 0x2B);
  add_mapping('g', 0x34);
  add_mapping('h', 0x33);
  add_mapping('i', 0x43);
  add_mapping('j', 0x3B);
  add_mapping('k', 0x42);
  add_mapping('l', 0x4B);
  add_mapping('m', 0x3A);
  add_mapping('n', 0x31);
  add_mapping('o', 0x44);
  add_mapping('p', 0x4D);
  add_mapping('q', 0x15);
  add_mapping('r', 0x2D);
  add_mapping('s', 0x1B);
  add_mapping('t', 0x2C);
  add_mapping('u', 0x3C);
  add_mapping('v', 0x2A);
  add_mapping('w', 0x1D);
  add_mapping('x', 0x22);
  add_mapping('y', 0x35);
  add_mapping('z', 0x1A);

  add_mapping('0', 0x45);
  add_mapping('1', 0x16);
  add_mapping('2', 0x1E);
  add_mapping('3', 0x26);
  add_mapping('4', 0x25);
  add_mapping('5', 0x2E);
  add_mapping('6', 0x36);
  add_mapping('7', 0x3D);
  add_mapping('8', 0x3E);
  add_mapping('9', 0x46);

  add_mapping(' ', 0x29);
  add_mapping('\n', 0x5A); // Enter

  top->clk = 0;
  top->clrn = 0;
  top->ps2_clk = 1;
  top->ps2_data = 1;
  step(10);
  top->clrn = 1;
  step(10);

  printf(
      "Simulation Ready. Type characters and press Enter (Ctrl+D to exit).\n");

  char c;
  while (std::cin.get(c)) {
    if (ascii_to_scan.find(c) != ascii_to_scan.end()) {
      uint8_t sc = ascii_to_scan[c];
      printf("Input: '%c', Sending Scan Code: %02X\n", c, sc);

      // Press
      send_ps2(sc);
      printf("  [Press]   Key Count: %d, Cur Key: %02x, ASCII: %02x (%c)\n",
             top->key_count, top->cur_key, top->ascii_key,
             (top->ascii_key >= 32 && top->ascii_key <= 126) ? top->ascii_key
                                                             : '?');

      // Release
      send_ps2(0xF0);
      send_ps2(sc);
      printf("  [Release] Key Count: %d, Cur Key: %02x, ASCII: %02x\n",
             top->key_count, top->cur_key, top->ascii_key);
    } else {
      if (c !=
          '\n') // Ignore newline if not mapped (though we mapped it to Enter)
        printf("Ignored unknown character: '%c' (0x%02x)\n", c, c);
    }
  }

  printf("Exiting Simulation\n");

  delete top;
  delete contextp;
  delete tfp;
  return 0;
}
