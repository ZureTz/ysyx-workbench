module lsu (
    input [31:0] address,
    input [31:0] r_data_in,
    input [31:0] w_data_in,
    input [2:0] funct3,
    input store,
    output [31:0] addr_out,
    output [31:0] r_data_out,
    output [31:0] w_data_out,
    output w_enable,
    output [3:0] w_mask
);

  // Address pass-through
  assign addr_out = address;

  // Low 2 bits of address for byte selection
  wire [1:0] low_addr = address[1:0];

  // Decode funct3 for operation type (using NOR gates as shown in diagram)
  wire is_b = ~funct3[0] & ~funct3[1];  // Byte operation (funct3[1:0] == 00)
  wire is_w = ~funct3[0] & funct3[1];  // Specific combination for word alignment(funct3[1:0] == 10)
  wire is_unsigned = funct3[2];  // Unsigned operation (LBU/LHU)

  // Write enable: only when store is active
  assign w_enable   = store;

  // ===== WRITE PATH =====
  // For byte store: shift the byte to the correct position based on address
  // For word store: pass all 32 bits
  assign w_data_out = (is_b) ? (w_data_in << {low_addr, 3'b0}) : w_data_in;

  // ===== READ PATH =====
  // Select byte from r_data_in based on low_addr
  wire [7:0] read_byte_00 = r_data_in[7:0];
  wire [7:0] read_byte_01 = r_data_in[15:8];
  wire [7:0] read_byte_10 = r_data_in[23:16];
  wire [7:0] read_byte_11 = r_data_in[31:24];

  wire [7:0] selected_read_byte = (low_addr == 2'b00) ? read_byte_00 :
                                  (low_addr == 2'b01) ? read_byte_01 :
                                  (low_addr == 2'b10) ? read_byte_10 :
                                                        read_byte_11;

  // Sign/zero extension for byte reads
  wire [31:0] extended_byte = is_unsigned ? {24'b0, selected_read_byte} :
                                           {{24{selected_read_byte[7]}}, selected_read_byte};

  // Select between byte-extended or word data
  assign r_data_out = is_b ? extended_byte : r_data_in;

  // ===== BYTE ENABLE GENERATION =====
  // Generate byte enable based on low_addr
  // For byte operations: only enable the specific byte at low_addr
  // For word operations: enable all 4 bytes

  wire [3:0] one_shift_low_addr = 4'b0001 << low_addr;

  // Byte enable logic based on operation type and address
  assign w_mask = is_b ? one_shift_low_addr : is_w ? 4'b1111 : 4'b0000;

endmodule
