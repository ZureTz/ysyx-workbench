#include <format>

#include "include/parser.h"

std::string parse_instruction(uint8_t inst) {
  uint8_t opcode = (inst >> 6) & 0x03;
  uint8_t rd = (inst >> 4) & 0x03;
  uint8_t rs1 = (inst >> 2) & 0x03;
  uint8_t rs2 = inst & 0x03;
  uint8_t imm = inst & 0x0f;
  uint8_t addr = (inst >> 2) & 0x0f;

  if (opcode == 0b00) { // add
    return std::format("add r{}, r{}, r{}", rd, rs1, rs2);
  }
  if (opcode == 0b10) { // li
    return std::format("li r{}, {}", rd, imm);
  }
  if (opcode == 0b11) { // bner0
    return std::format("bner0 {}, r{}", addr, rs2);
  }

  // opcode == 0b01 // nop/halt (add r0, r0, r0)
  return std::format("unknown ({:02x})", inst);
}
