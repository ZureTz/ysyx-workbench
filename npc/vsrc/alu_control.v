module alu_control (
    input [2:0] funct3,
    input [6:0] funct7,
    input op_imm,
    input op_reg,
    input lui,
    input load,
    input store,
    output op_add,
    output op_imm_direct
);

  // Decode funct3 to check if it's 000 (ADD/ADDI operation)
  wire funct3_is_add = (funct3 == 3'b000);

  // Check if funct7[5] indicates SUB operation (only for R-type)
  wire funct7_sub = funct7[5];

  // Logic for op_add:
  // 1. For op_imm (I-type): use ADD if funct3 == 000 (ADDI)
  // 2. For op_reg (R-type): use ADD if funct3 == 000 and funct7[5] == 0 (ADD, not SUB)
  // 3. For load instructions: always use ADD for address calculation
  // 4. For store instructions: always use ADD for address calculation
  wire add_for_imm = op_imm & funct3_is_add;
  wire add_for_reg = op_reg & funct3_is_add & ~funct7_sub;

  assign op_add = add_for_imm | add_for_reg | load | store;

  // op_imm_direct: directly use immediate without ALU operation (for LUI)
  assign op_imm_direct = lui;

endmodule
