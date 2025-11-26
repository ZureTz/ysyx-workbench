module program_counter (
    input clk,
    input rst,
    input [3:0] addr,
    input should_jump,
    output reg [3:0] pc
);

  always @(posedge clk or posedge rst) begin
    if (rst) begin
      pc <= 4'b0;
    end else if (should_jump) begin
      pc <= addr;
    end else begin
      pc <= pc + 1'b1;
    end
  end
endmodule
