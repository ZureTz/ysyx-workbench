#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"
#include "inst.h"

// CPU state
extern uint8_t pc;
extern uint8_t R[NREG];
extern int halt;

// CPU operations
void cpu_exec_once();

#endif
