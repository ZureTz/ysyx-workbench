module top (
    input clk,
    input rst
);

  // Wires for instruction memory
  wire [31:0] instruction_addr;
  wire [31:0] instruction;

  // Wires for data memory
  wire [31:0] data_addr;
  wire [31:0] data_r_data;
  wire [31:0] data_w_data;
  wire data_w_enable;
  wire [3:0] data_w_mask;

  // CPU Core
  core core_inst (
      .clk(clk),
      .reset(rst),
      .instruction(instruction),
      .r_data(data_r_data),
      .instruction_addr(instruction_addr),
      .addr(data_addr),
      .w_data(data_w_data),
      .w_enable(data_w_enable),
      .w_mask(data_w_mask)
  );

  // Instruction Memory
  memory imem (
      .addr(instruction_addr),
      .w_data(32'b0),
      .w_mask(4'b0),
      .w_enable(1'b0),
      .r_data(instruction)
  );

  // Data Memory
  memory dmem (
      .addr(data_addr),
      .w_data(data_w_data),
      .w_mask(data_w_mask),
      .w_enable(data_w_enable),
      .r_data(data_r_data)
  );

endmodule
