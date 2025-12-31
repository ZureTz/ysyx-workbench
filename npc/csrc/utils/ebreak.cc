#include <cstdio>

#include "ebreak.h"

// Global flag to indicate ebreak was executed
static bool ebreak_flag = false;
static int ebreak_code = 0;

// DPI-C function: Called by RTL when ebreak instruction is executed
extern "C" void set_ebreak(int exit_code) {
  ebreak_flag = true;
  ebreak_code = exit_code;
}

// Check if ebreak was executed
bool is_ebreak() { return ebreak_flag; }

// Get the exit code from the last ebreak
int get_ebreak_code() { return ebreak_code; }

// Reset ebreak flag
void reset_ebreak() {
  ebreak_flag = false;
  ebreak_code = 0;
}
