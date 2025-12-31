module register_file (
    input clk,
    input reset,
    input [4:0] rd,
    input [4:0] rs1,
    input [4:0] rs2,
    input [31:0] data,
    input write_en,
    output [31:0] rsa,
    output [31:0] rsb,
    output [31:0] a0  // x10/a0 register for halt exit code
);

  // 32 registers, each 32 bits wide
  reg [31:0] registers[32];

  // Read operations (combinational logic)
  // x0 is hardwired to 0
  assign rsa = (rs1 == 5'b0) ? 32'b0 : registers[rs1];
  assign rsb = (rs2 == 5'b0) ? 32'b0 : registers[rs2];
  assign a0 = registers[10];  // a0 is x10

  // Write operation (sequential logic)
  integer i;
  always @(posedge clk) begin
    if (reset) begin
      // Reset all registers to 0
      for (i = 0; i < 32; i = i + 1) begin
        registers[i] <= 32'b0;
      end
    end else if (write_en && rd != 5'b0) begin
      // Write to register rd if write_en is high and rd is not x0
      registers[rd] <= data;
    end
  end

endmodule
