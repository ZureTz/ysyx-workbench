#pragma once

// DPI-C function called by RTL when ebreak is executed
extern "C" void set_ebreak(int exit_code);

// Check if ebreak was executed
bool is_ebreak();

// Get the exit code from the last ebreak
int get_ebreak_code();

// Reset ebreak flag
void reset_ebreak();
