module top (
    input clk,
    input rst,
    input [7:0] instructions[16],
    output [3:0] pc,
    output [7:0] regs[4]
);

  // Signals
  wire [7:0] instruction;
  wire is_add, is_li, is_bner0;
  wire [1:0] rd, rs1, rs2;
  wire [3:0] imm, addr;
  wire [7:0] rdata1, rdata2;
  reg [7:0] wdata;
  wire wen;
  wire should_jump;
  wire [1:0] raddr1;

  // Program Counter
  program_counter u_pc (
      .clk(clk),
      .rst(rst),
      .addr(addr),
      .should_jump(should_jump),
      .pc(pc)
  );

  // Instruction Decoder
  assign instruction = instructions[pc];

  instruction_decoder u_dec (
      .instruction(instruction),
      .is_add(is_add),
      .is_li(is_li),
      .is_bner0(is_bner0),
      .rd(rd),
      .rs1(rs1),
      .rs2(rs2),
      .imm(imm),
      .addr(addr)
  );

  // Control Logic
  assign wen = is_add | is_li;

  // For bner0, we compare R[0] with R[rs2]. So raddr1 must be 0.
  // For add, raddr1 is rs1.
  assign raddr1 = is_bner0 ? 2'b00 : rs1;

  // ALU / Write Data Mux
  always_comb begin
    if (is_add) begin
      wdata = rdata1 + rdata2;
    end else if (is_li) begin
      wdata = {4'b0000, imm};
    end else begin
      wdata = 8'b0;
    end
  end

  // Branch Logic
  // bner0: if (R[0] != R[rs2]) PC = addr
  assign should_jump = is_bner0 && (rdata1 != rdata2);

  // Register File
  register_file u_rf (
      .clk(clk),
      .rst(rst),
      .rd(rd),
      .rs1(raddr1),
      .rs2(rs2),
      .write_data(wdata),
      .write_enable(wen),
      .read_data1(rdata1),
      .read_data2(rdata2),
      .regs(regs)
  );

endmodule
