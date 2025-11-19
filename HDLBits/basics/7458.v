module top_module (input p1a,
                   p1b,
                   p1c,
                   p1d,
                   p1e,
                   p1f,
                   output p1y,
                   input p2a,
                   p2b,
                   p2c,
                   p2d,
                   output p2y);
    
    wire p1_and_abc, p1_and_def;
    assign p1_and_abc = p1a & p1b & p1c;
    assign p1_and_def = p1d & p1e & p1f;
    assign p1y        = p1_and_abc | p1_and_def;
    
    wire p2_and_ab, p2_and_cd;
    assign p2_and_ab = p2a & p2b;
    assign p2_and_cd = p2c & p2d;
    assign p2y       = p2_and_ab | p2_and_cd;
    
endmodule
