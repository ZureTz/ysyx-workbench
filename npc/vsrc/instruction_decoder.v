module instruction_decoder (
    input [31:0] instruction,
    output [4:0] rd,
    output [2:0] funct3,
    output [4:0] rs1,
    output [4:0] rs2,
    output [6:0] funct7,
    output [31:0] immediate,
    output op_imm,
    output op_reg,
    output jalr,
    output lui,
    output load,
    output store,
    output ebreak
);

  // Field extraction
  assign rd     = instruction[11:7];
  assign funct3 = instruction[14:12];
  assign rs1    = instruction[19:15];
  assign rs2    = instruction[24:20];
  assign funct7 = instruction[31:25];

  wire [6:0] opcode = instruction[6:0];

  // Opcode decoding
  assign op_imm = (opcode == 7'h13);
  assign op_reg = (opcode == 7'h33);
  assign jalr   = (opcode == 7'h67);
  assign lui    = (opcode == 7'h37);
  assign load   = (opcode == 7'h03);
  assign store  = (opcode == 7'h23);

  // ebreak instruction: 0x00100073
  assign ebreak = (instruction == 32'h00100073);

  // Immediate generation
  wire [31:0] I_immediate;
  wire [31:0] U_immediate;
  wire [31:0] S_immediate;

  immediate_generator imm_gen (
      .instruction(instruction),
      .I_immediate(I_immediate),
      .U_immediate(U_immediate),
      .S_immediate(S_immediate)
  );

  // Mux logic based on Logisim implementation
  // Mux 1: Selects between I_immediate (0) and U_immediate (1) based on lui
  wire [31:0] mux1_out = lui ? U_immediate : I_immediate;

  // Mux 2: Selects between mux1_out (0) and S_immediate (1) based on store
  assign immediate = store ? S_immediate : mux1_out;

endmodule

module immediate_generator (
    input  [31:0] instruction,
    output [31:0] I_immediate,
    output [31:0] U_immediate,
    output [31:0] S_immediate
);
  // I-type immediate: imm[11:0] = inst[31:20], sign-extended
  assign I_immediate = {{20{instruction[31]}}, instruction[31:20]};

  // U-type immediate: imm[31:12] = inst[31:12], imm[11:0] = 0
  assign U_immediate = {instruction[31:12], 12'b0};

  // S-type immediate: imm[11:5] = inst[31:25], imm[4:0] = inst[11:7], sign-extended
  assign S_immediate = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};
endmodule
