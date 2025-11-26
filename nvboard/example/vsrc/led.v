module led (
    input clk,
    input rst,
    input [4:0] btn,
    input [7:0] sw,
    output [15:0] ledr
);
  parameter CLK_NUM = 10000000;

  reg [31:0] count;
  reg [ 7:0] led;

  always @(posedge clk) begin
    if (rst) begin
      led   <= 1;
      count <= 0;
    end else begin
      if (count == 0) led <= {led[6:0], led[7]};
      count <= (count >= CLK_NUM ? 32'b0 : count + 1);
    end
  end

  // assign ledr = {led[7:5], led[4:0] ^ btn, sw};
  assign ledr = {led[7:5], led[4:0] ^ btn, led[7:5], led[4:0] ^ btn};
endmodule
