module graycode(input [63:0] B,   // 输入64位二进制数
                output [63:0] G); // 输出64位格雷码
    assign G = B ^ (B >> 1);  // 通过异或操作实现格雷码转换
endmodule
