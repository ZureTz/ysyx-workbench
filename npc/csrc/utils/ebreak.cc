#include <cstdio>

#include "ebreak.h"

// Global flag to indicate ebreak was executed
static bool ebreak_flag = false;

// DPI-C function: Called by RTL when ebreak instruction is executed
extern "C" void set_ebreak() { ebreak_flag = true; }

// Check if ebreak was executed
bool is_ebreak() { return ebreak_flag; }

// Reset ebreak flag
void reset_ebreak() { ebreak_flag = false; }
