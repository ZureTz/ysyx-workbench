module core (
    input clk,
    input reset,
    input [31:0] instruction,
    input [31:0] r_data,
    output [31:0] instruction_addr,
    output [31:0] addr,
    output [31:0] w_data,
    output w_enable,
    output [3:0] w_mask
);

  // Instruction decoder outputs
  wire [4:0] rd;
  wire [2:0] funct3;
  wire [4:0] rs1;
  wire [4:0] rs2;
  wire [6:0] funct7;
  wire [31:0] immediate;
  wire op_imm;
  wire op_reg;
  wire jalr;
  wire lui;
  wire load;
  wire store;
  wire ebreak;

  // Register file outputs
  wire [31:0] rsa;
  wire [31:0] rsb;
  wire [31:0] reg_a0;  // a0 register for halt exit code

  // Write enable: write to register for these instruction types
  wire reg_write_en = jalr | op_imm | op_reg | lui | load;

  // Mux for second operand: select immediate or register
  wire use_immediate = jalr | op_imm | lui | load | store;
  wire [31:0] rsb_or_imm = use_immediate ? immediate : rsb;

  // Instruction Decoder
  instruction_decoder decoder (
      .instruction(instruction),
      .rd(rd),
      .funct3(funct3),
      .rs1(rs1),
      .rs2(rs2),
      .funct7(funct7),
      .immediate(immediate),
      .op_imm(op_imm),
      .op_reg(op_reg),
      .jalr(jalr),
      .lui(lui),
      .load(load),
      .store(store),
      .ebreak(ebreak)
  );

  // Pre-declare wires for register file writeback data
  wire [31:0] alu_result;
  wire [31:0] lsu_r_data_out;
  wire [31:0] result_or_r_data;
  wire [31:0] static_next_pc;
  wire [31:0] reg_writeback_data;

  // Register File
  register_file regfile (
      .clk(clk),
      .reset(reset),
      .rd(rd),
      .rs1(rs1),
      .rs2(rs2),
      .data(reg_writeback_data),
      .write_en(reg_write_en),
      .rsa(rsa),
      .rsb(rsb),
      .a0(reg_a0)
  );

  // ALU Control outputs
  wire alu_op_add;
  wire alu_op_imm_direct;

  // ALU Control
  alu_control alu_ctrl (
      .funct3(funct3),
      .funct7(funct7),
      .op_imm(op_imm),
      .op_reg(op_reg),
      .lui(lui),
      .load(load),
      .store(store),
      .op_add(alu_op_add),
      .op_imm_direct(alu_op_imm_direct)
  );

  // ALU
  alu alu_inst (
      .rsa(rsa),
      .rsb_or_imm(rsb_or_imm),
      .op_add(alu_op_add | jalr),
      .op_sub(1'b0),  // Not yet implemented in alu_control
      .op_imm_direct(alu_op_imm_direct),
      .result(alu_result)
  );

  // LSU
  lsu lsu_inst (
      .address(alu_result),
      .r_data_in(r_data),
      .w_data_in(rsb),
      .funct3(funct3),
      .store(store),
      .addr_out(addr),
      .r_data_out(lsu_r_data_out),
      .w_data_out(w_data),
      .w_enable(w_enable),
      .w_mask(w_mask)
  );

  // Mux for writeback data from ALU/LSU
  assign result_or_r_data = load ? lsu_r_data_out : alu_result;

  // Program Counter
  wire [31:0] pc_output;
  program_counter pc_inst (
      .clk(clk),
      .rst(reset),
      .jmp_addr(result_or_r_data),
      .is_jalr(jalr),
      .pc(pc_output),
      .static_next_pc(static_next_pc)
  );

  // Connect PC output to instruction_addr
  assign instruction_addr   = pc_output;

  // Mux for register file writeback data
  assign reg_writeback_data = jalr ? static_next_pc : result_or_r_data;

  // DPI-C function for ebreak
  import "DPI-C" function void set_ebreak(input int exit_code);

  // Call set_ebreak when ebreak instruction is executed
  always @(posedge clk) begin
    if (ebreak) begin
      set_ebreak(reg_a0);
    end
  end

endmodule  //core
