module program_counter (
    input             clk,
    input             rst,
    input      [31:0] jmp_addr,           // jump address (from ALU)
    input             is_jalr,            // whether to jump (from control signals)
    output reg [31:0] pc = 32'h80000000,  // current PC (InstructionAddr)
    output     [31:0] static_next_pc      // static next PC (always pc+4)
);

  wire [31:0] next_pc;  // dynamically calculated next PC

  // static_next_pc is always pc + 4
  assign static_next_pc = pc + 32'd4;

  // next_pc dynamically selects jump address or sequential address based on is_jalr
  assign next_pc = is_jalr ? jmp_addr : static_next_pc;

  // sequential logic: update PC on rising edge of clock
  always @(posedge clk) begin
    if (rst) begin
      pc <= 32'h80000000;  // reset PC to 0x80000000
    end else begin
      pc <= next_pc;  // update PC to dynamically calculated next value
    end
  end

endmodule
