module alu (
    input [31:0] rsa,
    input [31:0] rsb_or_imm,
    input op_add,
    input op_sub,
    input op_imm_direct,
    output [31:0] result
);

  // Arithmetic operations
  wire [31:0] add_result = rsa + rsb_or_imm;
  wire [31:0] sub_result = rsa - rsb_or_imm;

  // Three-stage multiplexer chain as shown in the diagram
  // Stage 1: Select based on op_add
  wire [31:0] mux1_out = op_add ? add_result : 32'b0;

  // Stage 2: Select based on op_sub
  wire [31:0] mux2_out = op_sub ? sub_result : mux1_out;

  // Stage 3: Select based on op_imm_direct (for LUI instruction)
  assign result = op_imm_direct ? rsb_or_imm : mux2_out;

endmodule
