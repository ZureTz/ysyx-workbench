#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

// Memory state
extern uint8_t M[NMEM];

// Memory operations
void memory_init();
uint8_t memory_read(uint8_t addr);
void memory_write(uint8_t addr, uint8_t data);

#endif
