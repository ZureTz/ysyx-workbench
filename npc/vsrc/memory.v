module memory (
    input [31:0] addr,
    input [31:0] w_data,
    input [3:0] w_mask,
    input w_enable,
    input [31:0] pc_debug,
    input is_instr,
    output reg [31:0] r_data
);

  // Import DPI-C functions for memory access
  import "DPI-C" function int pmem_read(
    input int raddr,
    input int pc,
    input bit is_instruction
  );
  import "DPI-C" function void pmem_write(
    input int  waddr,
    input int  wdata,
    input byte wmask,
    input int  pc
  );

  // Memory read/write logic
  always_comb begin
    // Always read from memory (address aligned to 4 bytes)
    r_data = pmem_read(addr, pc_debug, is_instr);

    // Write to memory when w_enable is high
    if (w_enable) begin
      pmem_write(addr, w_data, {4'b0, w_mask[3:0]}, pc_debug);
    end
  end

endmodule
