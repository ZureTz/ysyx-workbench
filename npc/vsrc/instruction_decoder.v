module instruction_decoder (
    input [7:0] instruction,
    output is_add,
    output is_li,
    output is_bner0,
    output [1:0] rd,
    output [1:0] rs1,
    output [1:0] rs2,
    output [3:0] imm,
    output [3:0] addr
);

  wire [1:0] opcode = instruction[7:6];

  assign is_add   = (opcode == 2'b00);
  assign is_li    = (opcode == 2'b10);
  assign is_bner0 = (opcode == 2'b11);

  assign rd   = instruction[5:4];
  assign rs1  = instruction[3:2];
  assign rs2  = instruction[1:0];
  assign imm  = instruction[3:0];
  assign addr = instruction[5:2];

endmodule
