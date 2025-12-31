#pragma once

// DPI-C function called by RTL when ebreak is executed
extern "C" void set_ebreak();

// Check if ebreak was executed
bool is_ebreak();

// Reset ebreak flag
void reset_ebreak();
