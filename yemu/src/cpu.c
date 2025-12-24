#include "cpu.h"
#include "memory.h"

uint8_t pc = 0;       // PC, C语言中没有4位的数据类型, 我们采用8位类型来表示
uint8_t R[NREG] = {}; // 寄存器
int halt = 0;         // 结束标志

// 执行一条指令
void cpu_exec_once() {
  // Instruction fetch
  inst_t this;
  this.inst = memory_read(pc);

  // Instruction decode and execute
  switch (this.rtype.op) {
  case 0b0000: { // mov rt, rs
    decoded_rtype_t decoded = decode_rtype(this);
    R[decoded.rt] = R[decoded.rs];
    break;
  }
  case 0b0001: { // add rt, rs
    decoded_rtype_t decoded = decode_rtype(this);
    R[decoded.rt] += R[decoded.rs];
    break;
  }
  case 0b1110: { // load addr
    decoded_mtype_t decoded = decode_mtype(this);
    R[0] = memory_read(decoded.addr);
    break;
  }
  case 0b1111: { // store addr
    decoded_mtype_t decoded = decode_mtype(this);
    memory_write(decoded.addr, R[0]);
    break;
  }
  default:
    printf("Invalid instruction with opcode = %x, halting...\n", this.rtype.op);
    halt = 1;
    break;
  }

  // Instruction write-back (update PC)
  pc++; // 更新PC
}
