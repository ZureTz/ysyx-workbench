// 实现一个 8-3 优先编码器并在七段数码管上显示
// 输入为8bit数据X[7:0]，高位表示有效，根据该八位二进制数进行高位优先编码成3比特编码，并根据是否有输入增加一位输入指示位， 即 8 个输入全 0 时指示位为 0，有任何一个输入为 1 时指示位为 1。 将此结果跟据七段数码管的显示进行译码，将二进制的优先编码结果以十进制的形式显示在数码管上。编码器的使能端为高有效。如果非使能时指示位为0，且数码管全暗。
// 例:我们从 X[7:0] 输入 00001110，因为我们设计的是一个高位优先 的优先编码器，从左(高位)开始，第一位为 1 的是第 3 号位，那么优先编 码器的编码二进制结果就为 011。再对这个数值跟据七段数码管的显示进行译码，此时应显示为 “3”，输出F[6:0] 应该译码为7'b0110000(注意高低位顺 序)，那么在七段数码管上就会显示 “3” 这个字符。同时应将指示位也置为 1。
// 实现细节要求：使能端无效时数码管全暗，指示位为0。使能端有效时，如果无输入，数码管全暗，指示位为0。

// 建议使用模块化设计，单独写一个数码管编码模块。

module decoder83(input [7:0] X,
                 input en,
                 output reg valid,
                 output reg [6:0] F);

reg [3:0] bcd_in;
wire [6:0] seg_out;

bcd7seg u_seg(
.bcd(bcd_in),
.seg(seg_out)
);

always @(*) begin
    valid  = 0;
    bcd_in = 0;
    F      = 7'b1111111; // Default dark (all 1s for active low)
    
    if (en) begin
        if (X == 0) begin
            valid = 0;
            F     = 7'b1111111; // Dark
            end else begin
            valid                 = 1;
            if (X[7]) bcd_in      = 4'd7;
            else if (X[6]) bcd_in = 4'd6;
            else if (X[5]) bcd_in = 4'd5;
            else if (X[4]) bcd_in = 4'd4;
            else if (X[3]) bcd_in = 4'd3;
            else if (X[2]) bcd_in = 4'd2;
            else if (X[1]) bcd_in = 4'd1;
            else if (X[0]) bcd_in = 4'd0;
            
            F = seg_out;
        end
        end else begin
        valid = 0;
        F     = 7'b1111111; // Dark
    end
end

endmodule


module bcd7seg(
    input  [3:0] bcd, //initializing bcd as an 4 bit input signal
    output [6:0] seg //initializing seg as an 8 bit output signal
    );
    
    // wire [6:0] seg; // Removed redundant declaration
    
    assign seg[0] = ((~bcd[3])&(~bcd[2])&(~bcd[1])&bcd[0]) | ((~bcd[3])&bcd[2]&(~bcd[1])&(~bcd[0])); //Logical expression for segment0 (Segment A)
    
    assign seg[1] = ((~bcd[3])&bcd[2]&(~bcd[1])&bcd[0]) | ((~bcd[3])&bcd[2]&bcd[1]&(~bcd[0])); //Logical expression for segment1 (Segment B)
    
    assign seg[2] = ((~bcd[3])&(~bcd[2])&bcd[1]&(~bcd[0])); //Logical expression for segment2 (Segment C)
    
    assign seg[3] = ((~bcd[3])&(~bcd[2])&(~bcd[1])&bcd[0]) | ((~bcd[3])&bcd[2]&(~bcd[1])&(~bcd[0])) | ((~bcd[3])&bcd[2]&bcd[1]&bcd[0]); //Logical expression for segment3 (Segment D)
    
    assign seg[4] = ((~bcd[3])&bcd[0]) | ((~bcd[3])&bcd[2]&(~bcd[1])) | (~(bcd[2])&(~bcd[1])&bcd[0]); //Logical expression for segment4 (Segment E)
    
    assign seg[5] = ((~bcd[3])&(~bcd[2])&bcd[0]) | ((~bcd[3])&(~bcd[2])&bcd[1]) | ((~bcd[3])&bcd[1]&bcd[0]); //Logical expression for segment5 (Segment F)
    
    assign seg[6] = ((~bcd[3])&(~bcd[2])&(~bcd[1]))|((~bcd[3])&bcd[2]&bcd[1]&bcd[0]); //Logical expression for segment6 (Segment G)
    
endmodule
