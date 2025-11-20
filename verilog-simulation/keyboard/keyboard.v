module keyboard(
    input clk,
    input clrn,
    input ps2_clk,
    input ps2_data,
    output reg [7:0] key_count,
    output reg [7:0] cur_key,
    output [7:0] ascii_key
);

    wire [7:0] keydata;
    wire ready;
    reg nextdata_n;
    /* verilator lint_off UNUSEDSIGNAL */
    wire overflow;
    /* verilator lint_on UNUSEDSIGNAL */

    // Instantiate provided modules
    scancode_ram myram(clk, cur_key, ascii_key);
    ps2_keyboard mykey(clk, clrn, ps2_clk, ps2_data, keydata, ready, nextdata_n, overflow);

    // State machine definitions
    localparam S_IDLE = 0;
    localparam S_READ = 1;
    localparam S_PROCESS = 2;
    localparam S_WAIT_BREAK = 3;
    localparam S_READ_BREAK = 4;
    localparam S_PROCESS_BREAK = 5;

    reg [2:0] state;
    reg [7:0] temp_data;

    always @(posedge clk) begin
        if (!clrn) begin
            key_count <= 0;
            cur_key <= 0;
            state <= S_IDLE;
            nextdata_n <= 1;
        end else begin
            case (state)
                S_IDLE: begin
                    nextdata_n <= 1;
                    if (ready) begin
                        state <= S_READ;
                        nextdata_n <= 0; // Signal to read
                    end
                end
                S_READ: begin
                    nextdata_n <= 1; // Deassert read
                    temp_data <= keydata;
                    state <= S_PROCESS;
                end
                S_PROCESS: begin
                    if (temp_data == 8'hF0) begin
                        state <= S_WAIT_BREAK;
                    end else begin
                        // Make code
                        if (temp_data != cur_key) begin
                            cur_key <= temp_data;
                            key_count <= key_count + 1;
                        end
                        state <= S_IDLE;
                    end
                end
                S_WAIT_BREAK: begin
                    if (ready) begin
                        state <= S_READ_BREAK;
                        nextdata_n <= 0;
                    end
                end
                S_READ_BREAK: begin
                    nextdata_n <= 1;
                    temp_data <= keydata;
                    state <= S_PROCESS_BREAK;
                end
                S_PROCESS_BREAK: begin
                    if (temp_data == cur_key) begin
                        cur_key <= 0;
                    end
                    state <= S_IDLE;
                end
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule

//standard ps2 interface, you can keep this
module ps2_keyboard(clk,clrn,ps2_clk,ps2_data,data,ready,nextdata_n,overflow);
    input clk,clrn,ps2_clk,ps2_data;
     input nextdata_n;
    output [7:0] data;
    output reg ready;
    output reg overflow;     // fifo overflow  
    // internal signal, for test
    reg [9:0] buffer;        // ps2_data bits
    reg [7:0] fifo[7:0];     // data fifo
     reg [2:0] w_ptr,r_ptr;   // fifo write and read pointers    
    reg [3:0] count;  // count ps2_data bits
    // detect falling edge of ps2_clk
    reg [2:0] ps2_clk_sync;
    
    always @(posedge clk) begin
        ps2_clk_sync <=  {ps2_clk_sync[1:0],ps2_clk};
    end
    wire sampling = ps2_clk_sync[2] & ~ps2_clk_sync[1];
    
    always @(posedge clk) begin
        if (clrn == 0) begin // reset 
            count <= 0; w_ptr <= 0; r_ptr <= 0; overflow <= 0; ready<= 0;
        end 
        else if (sampling) begin
            if (count == 4'd10) begin
                if ((buffer[0] == 0) &&  // start bit
                    (ps2_data)       &&  // stop bit
                    (^buffer[9:1])) begin      // odd  parity
                    fifo[w_ptr] <= buffer[8:1];  // kbd scan code
                    w_ptr <= w_ptr+3'b1;
                    ready <= 1'b1;
                    overflow <= overflow | (r_ptr == (w_ptr + 3'b1));
                end
                count <= 0;     // for next
            end else begin
                buffer[count] <= ps2_data;  // store ps2_data 
                count <= count + 3'b1;
            end      
        end
        if ( ready ) begin // read to output next data
                if(nextdata_n == 1'b0) //read next data
                begin
                   r_ptr <= r_ptr + 3'b1; 
                    if(w_ptr==(r_ptr+1'b1)) //empty
                         ready <= 1'b0;
                end           
        end
    end
    assign data = fifo[r_ptr];
endmodule

//simple scancode converter
module scancode_ram(clk, addr,outdata);
input clk;
input [7:0] addr;
output reg [7:0] outdata;
//Do not change the name of this ram, testbench will initialize this
/* verilator lint_off UNDRIVEN */
reg [7:0] ascii_tab[255:0];
/* verilator lint_on UNDRIVEN */
always @(posedge clk)
begin
      outdata <= ascii_tab[addr];
end
endmodule
