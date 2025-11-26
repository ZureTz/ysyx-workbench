module register_file (
    input clk,
    input rst,
    input [1:0] rd,
    input [1:0] rs1,
    input [1:0] rs2,
    input [7:0] write_data,
    input write_enable,
    output [7:0] read_data1,
    output [7:0] read_data2,
    output [7:0] regs[4]
);

  reg [7:0] registers[4];

  assign regs = registers;

  // Initialize registers to zero on reset
  integer i;
  always @(posedge clk or posedge rst) begin
    if (rst) begin
      for (i = 0; i < 4; i = i + 1) begin
        registers[i] <= 8'b0;
      end
    end else if (write_enable) begin
      registers[rd] <= write_data;
    end
  end

  assign read_data1 = registers[rs1];
  assign read_data2 = registers[rs2];
endmodule
