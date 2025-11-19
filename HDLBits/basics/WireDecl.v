`default_nettype none
module top_module(
    input a,
    input b,
    input c,
    input d,
    output out,
    output out_n   ); 

    wire a_and_b;
    assign a_and_b = a & b;

    wire c_and_d;
    assign c_and_d = c & d;

    wire and_b_or_c_and_d;
    assign and_b_or_c_and_d = a_and_b | c_and_d;

    assign out = and_b_or_c_and_d;
    assign out_n = ~and_b_or_c_and_d;

endmodule
